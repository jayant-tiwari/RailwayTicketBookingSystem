#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(void){

int fd;
/*	int fd;
	struct{
		char tnum[5];
		int seats;
	}trn;*/
	/*strcpy(trn.tnum,"10040");
	trn.seats=10;*/
	fd = open("userDB",O_CREAT|O_RDWR,0744);
	//fd=open("trains",O_RDWR);
        /*lseek(fd,sizeof(trn),SEEK_CUR);
	write(fd,&trn,sizeof(trn));
	close(fd);*/
	/*fd = open("user",O_RDONLY);
	read(fd,&usr,sizeof(usr));
	printf("User=%s,pass=%s\n",usr.username,usr.pass);*/
	//fd=open("trains",O_RDONLY);
	/*while(read(fd,&trn,sizeof(trn))>0)
	{
	 printf("%s\n",trn.tnum);
	 printf("%d\n",trn.seats);
	}*/
	close(fd);
	return 0;
}
