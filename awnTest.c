#include <stdio.h>
#include <awn/awn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define READ_SIZE 4096
int bufSrcfd;
void* bufSrc;
unsigned long MAXMEMCPY;

void main(){
    int err, writefd;
    struct stat st;
	void* buf;

	buf = malloc(4097);
	if(!buf){
		printf("malloc buf errno.\n");
	}

    bufSrcfd = ts_open("/home/xzjin/FIO_COPY_SRC", O_RDONLY);
    err = errno;
    if(bufSrcfd<=0){
        printf("Open buf src file error, %s\n", strerror(err));
        return;
    }

	writefd = ts_open("/home/xzjin/tmp.txt", O_RDWR);
    err = errno;
    if(writefd<=0){
        printf("Open buf write file error, %s\n", strerror(err));
        return;
    }

    if(fstat(bufSrcfd, &st)){
        err = errno;
		printf("Get file stat failed %s.\n", strerror(err));
        return;
    }

    MAXMEMCPY = st.st_size;
    bufSrc = ts_mmap(0, MAXMEMCPY, PROT_READ,
        MAP_PRIVATE, bufSrcfd, 0);

	while(1){
		lseek(bufSrcfd, 0, SEEK_SET);
		ts_read(bufSrcfd, buf, READ_SIZE);
		ts_pwrite(writefd, buf, READ_SIZE, 0);
		sleep(1);
	}
}