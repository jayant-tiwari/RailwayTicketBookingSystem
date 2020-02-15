#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define port_no 8000
#define USER 0
#define ADMIN 2
#define UNAUTH_USER -1
#define RESPONSE_BYTES 512
#define REQUEST_BYTES 512
#define linesInMS 5
#define EXIT -1

typedef struct login{
	char username[100];
	char password[100];
//	char type;
}user;
typedef struct trainInfo
{
	int tid;
	char from[10];
	char to[10];
	char dep[10];
	char arr[10];
	int seats;
}train;
typedef struct bookedTicket{
	int train;
	int seat;
}bt;

void sendMsgtoClient(int clientFD, char *str) {
    int numPacketsToSend = (strlen(str)-1)/RESPONSE_BYTES + 1;
    int n = write(clientFD, &numPacketsToSend, sizeof(int));
    char *msgToSend = (char*)malloc(numPacketsToSend*RESPONSE_BYTES);
    strcpy(msgToSend, str);
    int i;
    for(i = 0; i < numPacketsToSend; ++i) {
        int n = write(clientFD, msgToSend, RESPONSE_BYTES);
        msgToSend += RESPONSE_BYTES;
    }
}
char* recieveMsgFromClient(int clientFD) {
    int numPacketsToReceive = 0;
    int n = read(clientFD, &numPacketsToReceive, sizeof(int));
    if(n <= 0) {
        shutdown(clientFD, SHUT_WR);
        return NULL;
    }
    char *str = (char*)malloc(numPacketsToReceive*REQUEST_BYTES);
    memset(str, 0, numPacketsToReceive*REQUEST_BYTES);
    char *str_p = str;
    int i;
    for(i = 0; i < numPacketsToReceive; ++i) {
        int n = read(clientFD, str, REQUEST_BYTES);
        str = str+REQUEST_BYTES;
    }
    return str_p;
}
void getupcli(char *username,char *password,int client_fd)
{
	char *ruser,*rpass;
	sendMsgtoClient(client_fd,"Enter Username: ");
	ruser=recieveMsgFromClient(client_fd);

	sendMsgtoClient(client_fd,"Enter Password: ");
	rpass=recieveMsgFromClient(client_fd);

	int i=0;
	while(ruser[i]!='\0' && ruser[i]!='\n')
	{
		username[i]=ruser[i];
		i++;
	}

	username[i]='\0';

	i=0;
	while(rpass[i]!='\0' && rpass[i]!='\n')
	{
		password[i]=rpass[i];
		i++;
	}
	password[i]='\0';

}

void closeClient(int client_fd,char *str)
{
	sendMsgtoClient(client_fd,str);
	shutdown(client_fd,SHUT_RDWR);
}
void bookTicket(char *username,char *password,int client_fd)
{
	train t;
	bt current;
	int num,trainNo,seatNo,choice,fd;
	bool bookedTicket=false;
	char *temp=(char *)malloc(10);
	char *available=(char *)malloc(500);
	while(1)
	{
	fd=open("trainDB",O_RDONLY);
	strcpy(available," ");
        while(read(fd,&t,sizeof(t))>0)
        {
                num=t.tid;
                sprintf(temp,"%d",num);
                strcat(available,temp);
                strcat(available," From:");
                strcat(available,t.from);
                strcat(available," To:");
                strcat(available,t.to);
                strcat(available," DEP:");
                strcat(available,t.dep);
                strcat(available," ARR:");
                strcat(available,t.arr);
                strcat(available," Seats:");
                num=t.seats;
                sprintf(temp,"%d",num);
                strcat(available,temp);
                strcat(available,"\n");

        }
        close(fd);
        strcat(available,"Enter Train Number:");
	bookedTicket=false;
	sendMsgtoClient(client_fd,available);
	temp=recieveMsgFromClient(client_fd);
	trainNo=atoi(temp);
	fd=open("trainDB",O_RDWR);
	while(read(fd,&t,sizeof(t))>0)
	{
		if(trainNo==t.tid)
		{
			if(t.seats!=0)
			{
		                seatNo=t.seats;
                                t.seats=t.seats-1;
				lseek(fd,-1*sizeof(train),SEEK_CUR);
				write(fd,&t,sizeof(t));
				bookedTicket=true;
				close(fd);
				sendMsgtoClient(client_fd,"Tickets Booked");
				//temp=recieveMsgFromClient(client_fd);
				break;
			}
			else
			{
				sendMsgtoClient(client_fd,"No seats Press any key to continue");
				//temp=recieveMsgFromClient(client_fd);
				close(fd);
				break;
				
			}
		}
	}
	close(fd);
	if(bookedTicket)
	{
		fd=open(username,O_WRONLY | O_APPEND);
		current.train=trainNo;
		current.seat=seatNo;
		write(fd,&current,sizeof(current));
		close(fd);

	}
	sendMsgtoClient(client_fd,"Want to book more tickets: Yes-0 No-1");
	temp=recieveMsgFromClient(client_fd);
	choice=atoi(temp);
	if(choice!=0)
		break;
	}


}
void bookedHistory(char *username,int client_fd)
{

	char *temp=(char *)malloc(100);
	char *history=(char *)malloc(500);
	bt myhistory;
	int fd=open(username,O_RDONLY);
	while(read(fd,&myhistory,sizeof(myhistory))>0)
	{
	//	printf("Inside Booking History\n");
		sprintf(temp,"%d",myhistory.train);
		strcat(history,"Train number: ");
		strcat(history,temp);
		sprintf(temp,"%d",myhistory.seat);
		strcat(history," Seat number: ");
		strcat(history,temp);
		strcat(history,"\n");
	//	printf("Inside Booking History\n");

	}
	close(fd);
	strcat(history,"Press 0 to continue");
	sendMsgtoClient(client_fd,history);
	temp=recieveMsgFromClient(client_fd);

}
void changePassword(char *username,int client_fd)
{

	user change;
	int fd,count=0;
	char *newPass=(char *)malloc(100);
	char *confirmPass=(char *)malloc(100);
		fd=open("userDB",O_RDWR);
		while(read(fd,&change,sizeof(change))>0)
		{
			if(strcmp(change.username,username)==0)
			{
				while(count<3)
				{
				count++;
				sendMsgtoClient(client_fd,"Enter New Password:\n");
				newPass=recieveMsgFromClient(client_fd);
				sendMsgtoClient(client_fd,"Confirm Password:\n");
				confirmPass=recieveMsgFromClient(client_fd);
				if(strcmp(newPass,confirmPass)==0)
				{
					lseek(fd,-1*(sizeof(user)),SEEK_CUR);
					strcpy(change.password,newPass);
					write(fd,&change,sizeof(change));
							sendMsgtoClient(client_fd,"Password changed successfully");
					count=3;
					break;
				}
				else
				{
					sendMsgtoClient(client_fd,"Did not match");
				}
				}

			}
		}
	close(fd);
}

void userInterface(char *username,char *password,int client_fd)
{

	int choice=-1;
	char *buff;
	char options[]="\n1-Book Ticket\n2-Cancel Ticket\n3-Booked Ticket History\n4-Change Password\n5-Exit\n";
	while(choice!=5)
	{
	sendMsgtoClient(client_fd,options);
	buff=recieveMsgFromClient(client_fd);
	choice=atoi(buff);
	switch(choice)
	{
		case 1: bookTicket(username,password,client_fd);
			break;
		//case 2: cancelTicket(username,client_fd);
		//	break;
		case 3: bookedHistory(username,client_fd);
			break;
		case 4: changePassword(username,client_fd);
			break;
		case 5: break;

	}
	}
}
void checkUser(char *username, char *password, int client_fd)
{
	user current;
	bool flag=false;
	int fd=open("userDB",O_RDONLY);
	while((read(fd,&current,sizeof(current)))>0)
	{
		if(strcmp(current.username,username)==0 && strcmp(current.password,password)==0)
		{
			//sendMsgtoClient(client_fd,"Authenticated!\nWelcome back comrade");
			printf("Found you\n");
			flag=true;
			break;
		}
	}
	close(fd);
	if(flag==false)
		printf("Tresspassing\n");//sendMsgtoClient(client_fd,"TressPassing !");
	else
	{
		userInterface(username,password,client_fd);
	}
}

void addUser(char *username,char *password,int client_fd)
{
	bool already_exists=false;
	printf("%d\n",client_fd);
	user fresh;
	int fd=open("userDB",O_RDONLY);
	while(read(fd,&fresh,sizeof(fresh))>0)
	{
		if(strcmp(fresh.username,username)==0)
		{
			already_exists=true;
			break;
		}
	}
	close(fd);
	strcpy(fresh.username,username);
        strcpy(fresh.password,password);
	if(!already_exists)
	{
	int fd=open("userDB",O_WRONLY | O_APPEND);
	write(fd, &fresh, sizeof(fresh));
	close(fd);
	sendMsgtoClient(client_fd,"You are onbaord now! Press 0 to continue");
	fd=open(username,O_CREAT | O_RDWR,0744);
	close(fd);
	}
	else
		sendMsgtoClient(client_fd,"Username already exists! Press 0 to continue");
}

/*
void addAdmin(char *adminName,char *password,int client_fd)
{

	user admin;
	int fd=open("adminDB",O_RDONLY);
	while(read(fd,&admin,sizeof(admin))>0)
	{
		if(strcmp(admin.username,username)==0 && strcmp(admin.password,password)==0)
		{

		}
	}
}
*/
void talkToClient(int client_fd,int choice)
{
	char *username,*password;
	username=(char *)malloc(100);
	password=(char *)malloc(100);
	int utype;
	getupcli(username,password,client_fd);
	//utype=authorize(username,password);

	/*if(choice==4)
	{
	printf("%s\n",username);
	printf("%s\n",password);
	}*/
// 	char *str=(char *)malloc(sizeof(char)*60);
//	strcpy(str,"Thanks ");

	switch(choice)
	{
		case 4:
//			sendMsgtoClient(client_fd,"Welcome Aboard Commrade !");
			addUser(username,password,client_fd);
			//closeclient(client_fd,strcat(str,username));
			break;
		case 1:
			checkUser(username,password,client_fd);
        		//printf("%s\n",username);
        		//printf("%s\n",password);
			/*printf("ADMIN is IN \n");
			adminRequests(username, client_fd);
			closeclient(client_fd,strcat(str,username));*/
			break;

//		case 2: addAdmin(username,password,client_fd);


	/*	case UNAUTH_USER:
			closeclient(client_fd,"unauthorised");
			break;
		default:
			closeclient(client_fd,"unauthorised");
			break;*/
	}
}

void homePage(int client_fd)
{
	int choice=4;
	char options[]="\n 1-User Login \n 2-Admin Login \n 3-Agent Login \n 4-Register User \n 5-Exit \n";
	char *temp=NULL;
//	char *username=(char *)malloc(100);
//	char *password=(char *)malloc(100);
	int value;

	while(choice!=5)
	{
	char *buff=NULL;
	sendMsgtoClient(client_fd,options);
	buff=recieveMsgFromClient(client_fd);
	choice=atoi(buff);
	if(choice==5)
		break;
	if(choice==4)
	{
	talkToClient(client_fd,choice);
	temp=recieveMsgFromClient(client_fd);
	value=atoi(temp);
	if(value!=0)
		break;
	}
	if(choice==1)
	{
		talkToClient(client_fd,choice);

	}
	if(choice==2)
	{
		talkToClient(client_fd,choice);
	}
	}
	closeClient(client_fd,"exiting");
	
}

int main(int argc,char **argv)
{
	int sock_fd,client_fd;
	struct sockaddr_in serv_addr, cli_addr;

	memset((void*)&serv_addr, 0, sizeof(serv_addr));
	//port_no=8080;

	sock_fd=socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_port = htons(port_no);         //set the port number
	serv_addr.sin_family = AF_INET;             //setting DOMAIN
	serv_addr.sin_addr.s_addr = INADDR_ANY;     //permits any incoming IP

	if(bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
	    printf("Error on binding.\n");
	    exit(EXIT_FAILURE);
	}
	int reuse=1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
	listen(sock_fd, 5); 
	int clisize=sizeof(cli_addr);

	while(1) {
	    //blocking call
	    memset(&cli_addr, 0, sizeof(cli_addr));
	    if((client_fd = accept(sock_fd, (struct sockaddr*)&cli_addr,(socklen_t*)&clisize)) < 0) {
	        printf("Error on accept.\n");
	        exit(EXIT_FAILURE);
	    }

	    switch(fork()) {
	        case -1:
	            printf("Error in fork.\n");
	            break;
	        case 0: {
	            close(sock_fd);
	            homePage(client_fd);
	            exit(EXIT_SUCCESS);
	            break;
	        }
	        default:
	            close(client_fd);
	            break;
	    }
	}

}
