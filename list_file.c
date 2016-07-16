#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

//static int filter(const struct dirent *dirent) {
	/*	".", ".." skip
	if(!strcmp(dirent->d_name, ".") || !(strcmp(dirent->d_name, ".."))) {
	} else {
		printf("  %s() : %s\n", __FUNCTION__, dirent->d_name);
	} */

//	printf("");
//}
// dirent구조체의 d_type을 받아서 파일 타입 출력
char* get_filetype(char d_type) {
	if(d_type == DT_BLK) return "b";
	else if(d_type == DT_CHR) return "c";
	else if(d_type == DT_DIR) return "d";
	else if(d_type == DT_FIFO) return "p";
	else if(d_type == DT_LNK) return "l";
	else if(d_type == DT_REG) return "-";
	else if(d_type == DT_SOCK) return "s";
	else return "?";
}

// time구조체를 월/일 시:분 형태로 변
char *time_to_char(time_t time) {
	struct tm *pt_time;
	static char time_char[20];

	pt_time = localtime(&time);
	sprintf(time_char, "%02d월 %02d일 %02d:%02d  ", pt_time->tm_mon+1, pt_time->tm_mday, pt_time->tm_hour, pt_time->tm_min);

	return time_char;
}

void list_file(char *startdir, int depth, int leaf_depth) {
	struct dirent **filelist;
	struct stat stat1;
	int count, i;
	char *dir;
	int num_blanks = depth*2;
	char current_dir[1024];

	// leaf_depth에 도달하면 함수 종료
	if(depth > leaf_depth) {
		return;
	}
	
	// current directory 변경(상대경로 절대경로 문제가 생길 수 있기 때문)
	chdir(startdir);
	getcwd(current_dir, 1024);

	// start directory의 filelist 저장
	count = scandir(current_dir, &filelist, NULL, alphasort);
	
	// scandir 결과(filelist)를 출력하고 directory일 경우 재귀로 list_file호출
	for(i=0; i<count; i++) {
		// file의 정보 얻기(mac time을 얻기 위해)
		lstat(filelist[i]->d_name, &stat1);

		// directory일 경우 파일명의 마지막에 "/" 붙임
		if(filelist[i]->d_type == 4) {
			printf("%10d, %8s, %s, %s, %s,   %*s%s/\n", filelist[i]->d_ino, get_filetype(filelist[i]->d_type), time_to_char(stat1.st_atime), time_to_char(stat1.st_mtime), time_to_char(stat1.st_ctime), num_blanks, "", filelist[i]->d_name);
		} else {
			printf("%10d, %8s, %s, %s, %s,   %*s%s\n", filelist[i]->d_ino, get_filetype(filelist[i]->d_type), time_to_char(stat1.st_atime), time_to_char(stat1.st_mtime), time_to_char(stat1.st_ctime), num_blanks, "", filelist[i]->d_name);
		}

		// "."과 ".." directory는 재귀 호출에서 제외
		if((filelist[i]->d_type == 4) && strcmp(filelist[i]->d_name, ".") && strcmp(filelist[i]->d_name, "..")) {
			dir = filelist[i]->d_name;
			list_file(dir, depth+1, leaf_depth);
		}
	}

	chdir("..");
}

int main(int argc, char *argv[]) {
	char *startdir;
	char default_startdir[2] = ".";
	int leaf_depth;
	struct dirent **filelist;
	char tmp[1024];

	// argument가 3개일 때는 사용자가 target_dir과 depth를 설정할 수 있음.	
	if(argc == 3) {
		startdir = argv[1];
		leaf_depth = atoi(argv[2]);
	// argument가 2개일 때는 사용자가 target_dir만 설정 가능. depth는 default(= 0).
	} else if(argc == 2) {
		startdir = argv[1];
		leaf_depth = 0;
	// argument가 1개일 때는 target_dir : default("."), depth : default(= 0)
	} else if(argc == 1) {
		startdir = default_startdir;
		leaf_depth = 0;
	// 이외의 경우에는 사용법 출력
	} else {
		printf("[Usage] %s [target_dir] [depth]\n", argv[0]);
	}

	printf("%10s%10s%17s%17s%19s%14s\n", "d_ino", "d_type", "Access time", "Modify time", "Change time", "d_name");
	list_file(startdir, 0, leaf_depth);

	return 0;
}
