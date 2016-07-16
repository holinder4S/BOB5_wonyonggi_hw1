#include <stdio.h>
#include <io.h>
#include <time.h>
#include <stdlib.h>

void list_file(char *target_dir) {
	long files;
	struct _finddata_t file_attr;
	char target_dir_tmp[1024];

	sprintf(target_dir_tmp, "%s/*.*", target_dir);
	if((files = _findfirst(target_dir_tmp, &file_attr)) == -1) {
		printf("No files~\n");
	}
	else {
		do {
			char atime[30], ctime[30], wtime[30];
			ctime_s(atime, _countof(atime), &file_attr.time_access);
			ctime_s(ctime, _countof(ctime), &file_attr.time_create);
			ctime_s(wtime, _countof(wtime), &file_attr.time_write);
			printf("%.24s | %.24s | %.24s | %-12s\n", atime, ctime, wtime, file_attr.name);
		} while(_findnext(files, &file_attr) == 0);
	}
}

int main(int argc, char *argv[]) {
	char *target_dir;
	char default_startdir[2] = ".";

	if(argc != 2) {
		printf("[Usage] %s [target_dir]\n", argv[0]);
	} else {
		target_dir = argv[1];
	}

	printf("%-24s %-28s %-28s %12s\n", "[Access time]", "[Create time]", "[Write time]", "[File name]");
	list_file(target_dir);
}