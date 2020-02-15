#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct trainInfo
{
        int tid;
        char from[10];
        char to[10];
        char dep[10];
        char arr[10];
        int seats;
}train;

int main(void){

int fd;
/*	int fd;
	struct{
		char tnum[5];
		int seats;
	}trn;*/
	/*strcpy(trn.tnum,"10040");
	trn.seats=10;*/
	fd = open("trainDB",O_CREAT|O_RDWR,0744);
	close(fd);
	fd=open("trainDB",O_WRONLY | O_APPEND);
	train t,r;
 	t.tid=1234;
	strcpy(t.from,"DED");
	strcpy(t.to,"HDW");
	strcpy(t.dep,"23:00");
	strcpy(t.arr,"07:00");
	t.seats=10;
	write(fd,&t,sizeof(t));
	close(fd);
	fd=open("trainDB",O_WRONLY | O_APPEND);
	r.tid=4321;
	strcpy(r.from,"HDW");
	strcpy(r.to,"DED");
	strcpy(r.dep,"06:50");
	strcpy(r.arr,"02:00");
	r.seats=10;
	write(fd,&r,sizeof(r));
	close(fd);
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
	//close(fd);
	return 0;
}
