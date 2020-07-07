#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8686

int admin(int, char[]);
void add(int);
void modify(int);
void search(int);
void delete(int);

int agent(int, char[]);
void Abookticket(int, char[]);
void Aviewbks(int, char[]);
void Aupdatebk(int, char[]);

int customer(int, char[]);
void bookticket(int, char[]);
void viewbks(int, char[]);
void updatebk(int, char[]);
void cancelbk(int, char[]);

void irctc(int);

int main()
{
        int csock, ret, opt_display;
        struct sockaddr_in servaddr;
        char buf[50]; 
	char uid[20], pwd[20];

        csock = socket(AF_INET, SOCK_STREAM, 0);
        if(csock < 0)
        {
                printf("\n\t error in socket  creation : ");
                exit(0);
        }
        memset(&servaddr, '\0', sizeof(servaddr));
        servaddr.sin_family = AF_INET;
	 servaddr.sin_port = htons(PORT);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        ret = connect(csock, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if(ret < 0)
        {
                printf("\n\t error occured while connecting");
                exit(0);
        }
        while(1)
        {
         	irctc(csock);       
     	}
        return 0;
}

void irctc(int csock)
{
	int opt, type;
	char uid[30], pwd[30]; 
	system("clear");
	printf("\n\t 1.Login \n\t 2.Signup \n\t 3.Quit \n\t Your Responce : ");
	scanf("%d", &opt);
	switch(opt)
	{
		case 1:
		case 2:printf("\n\t Enter user id : ");
			scanf("%s", uid);
			printf("\n\t Enter password : ");
			scanf("%s", pwd);
			write(csock, &opt, sizeof(opt));
			write(csock, uid, sizeof(uid));
			write(csock, pwd, sizeof(pwd));
			read(csock, &type, sizeof(type));
			if(type == 1)
			{
				while(admin(csock, uid));
			}
			else if(type == 2)
			{
				while(agent(csock, uid));
			}
			else if(type == 3)
			{
				while(customer(csock, uid));
			}
			else
			{
				printf("\n\t Invalid credintials/Already Exists.......\n");
			}
			exit(0);
		case 3:exit(0);
	}
}

int admin(int csock, char uid[])
{
	int opt;
	//sleep(2);
	//system("clear");
	printf("\n\t 1.Add \n\t 2.Modify \n\t 3.Search \n\t 4.Delete \n\t 5.Quit \n\t your Response :");
	scanf("%d", &opt);
	write(csock, &opt, sizeof(opt));
	switch(opt)
	{
		case 1: add(csock);
			return 1;
		case 2: modify(csock);
			return 1;
		case 3: search(csock);
			return 1;
		case 4: delete(csock);
			return 1;
		case 5: printf("\n\t disconnected........");
			exit(0);
	}
	
	
}

void add(int csock)
{
	int opt, train_id, seats_avl;
	char uid[30], pwd[30], train[20];
	
		printf("\n\t Enter the option that you want to ADD : ");
		printf("\n\t 1.USER \n\t 2.AGENT \n\t 3.TRAIN \n\t 4.QUIT \n\tYOUR RESPONSE : ");
		scanf("%d", &opt);

		write(csock, &opt, sizeof(opt));
		if(opt == 1)
		{
			printf("\n\t Enter Customer id : ");
			scanf("%s", uid);
			printf("\n\t Enter password :");
			scanf("%s", pwd);

			write(csock, uid, sizeof(uid));
			write(csock, pwd, sizeof(pwd));
			printf("\n\t Added Successfully.....");
			
		}
		else if(opt == 2)
		{
			printf("\n\t Enter Agent id : ");
			scanf("%s", uid);
			printf("\n\t Enter password :");
			scanf("%s", pwd);
	
			write(csock, uid, sizeof(uid));
			write(csock, pwd, sizeof(pwd));
			printf("\n\t Added Successfully.....");
		}	
		else if(opt == 3)
		{
			printf("\n\t Enter Train ID : ");
			scanf("%d", &train_id);
			printf("\n\t Enter Train Name : ");
			scanf("%s", train);
			printf("\n\t Enter No of seats Available : ");
			scanf("%d", &seats_avl);
			
			write(csock, &train_id, sizeof(train_id));
			write(csock, train, sizeof(train));
			write(csock, &seats_avl, sizeof(seats_avl));
			
			printf("\n\t Added Succesfully...........");
		}	
		else if(opt == 4)
		{
			return;
		}
}

void modify(int csock)
{
	int opt, train_id, seats_avl;
	char uid[30], pwd[30], train[20];
	
		printf("\n\t Enter the option that you want to MODIFY : ");
		printf("\n\t 1.USER \n\t 2.AGENT \n\t 3.TRAIN \n\t 4.QUIT \n\tYOUR RESPONSE : ");
		scanf("%d", &opt);

		write(csock, &opt, sizeof(opt));
		if(opt == 1)
		{
			printf("\n\t Enter Customer id : ");
			scanf("%s", uid);
			printf("\n\t Enter NEW password :");
			scanf("%s", pwd);

			write(csock, uid, sizeof(uid));
			write(csock, pwd, sizeof(pwd));
			printf("\n\t Modified Successfully.....");
			
		}
		else if(opt == 2)
		{
			printf("\n\t Enter Agent id : ");
			scanf("%s", uid);
			printf("\n\t Enter NEW password :");
			scanf("%s", pwd);
	
			write(csock, uid, sizeof(uid));
			write(csock, pwd, sizeof(pwd));
			printf("\n\t Modified Successfully.....");
		}	
		else if(opt == 3)
		{
			char status;
			printf("\n\t Enter Train ID : ");
			scanf("%d", &train_id);
			printf("\n\t Enter No of seats to INC(Enter NO.)/DEC(Enter NO. with -sign)  : ");
			scanf("%d", &seats_avl);
			
			write(csock, &train_id, sizeof(train_id));
			write(csock, &seats_avl, sizeof(seats_avl));
			
			read(csock, &status, sizeof(status));
			if(status == 'S')
				printf("\n\t Modified Succesfully...........");
			else
				printf("\n\t No such train to modify.........");
		}	
		else if(opt == 4)
		{
			return;
		}
	
}
void search(int csock)
{
	int opt;
	char uid[30], status;
	struct user
	{
		char email[30];
		char pwd[30];
		int type;
	}u;
	struct train
	{
		int train_id;
		char train[20];
		int seats_avl;
		//int last_seatNo;
	}t;
	printf("\n\t Enter the option you want to search.......");
	printf("\n\t 1.user \n\t 2. agent.\n\t 3.train \n\t Enter Your Response : ");
	scanf("%d", &opt);
	write(csock, &opt, sizeof(opt));
	switch(opt)
	{
		case 1: printf("\n\t Enter the user Id :");
			scanf("%s", uid);
			write(csock, uid, sizeof(uid));
			read(csock, &status, sizeof(status));
			if(status == 'S')
			{
				read(csock, &u, sizeof(u));
				printf("\n\t User : %s, password : %s", u.email, u.pwd);				
			}
			else
			{
				printf("\n\t No Such user......");
			}
			break;
		case 2: printf("\n\t Enter the user Id :");
			scanf("%s", uid);
			write(csock, uid, sizeof(uid));
			read(csock, &status, sizeof(status));
			if(status == 'S')
			{
				read(csock, &u, sizeof(u));
				printf("\n\t AgentID : %s, password : %s", u.email, u.pwd);				
			}
			else
			{
				printf("\n\t No Such Agent......");
			}
			break;
		case 3: printf("\n\t Enter the train name :");
			scanf("%s", uid);
			write(csock, uid, sizeof(uid));
			read(csock, &status, sizeof(status));
			if(status == 'S')
			{
				read(csock, &t, sizeof(t));
				printf("\n\t trainID : %d, Train Name : %s, NO. of seats Available : %d",  t.train_id, t.train, t.seats_avl);
			}
			else
			{
				printf("\n\t No Such Train......");
			}
			break;
		default : printf("\n\t Invalid option....");
	}
}
void delete(int csock)
{
	int opt, train_id, seats_avl;
	char status;
	char uid[30], pwd[30], train[20];
	
		printf("\n\t Enter the option that you want to Delete : ");
		printf("\n\t 1.USER \n\t 2.AGENT \n\t 3.TRAIN \n\t 4.QUIT \n\tYOUR RESPONSE : ");
		scanf("%d", &opt);

		write(csock, &opt, sizeof(opt));
		if(opt == 1)
		{
			printf("\n\t Enter Customer id : ");
			scanf("%s", uid);
			write(csock, uid, sizeof(uid));
			read(csock, &status, sizeof(status));
			if(status == 'S')
				printf("\n\t Deleted Successfully.....");
			else
				printf("\n\t No such user to delete......");
			
		}
		else if(opt == 2)
		{
			printf("\n\t Enter Agent id : ");
			scanf("%s", uid);
			write(csock, uid, sizeof(uid));
			
			read(csock, &status, sizeof(status));
			if(status == 'S')
				printf("\n\t Deleted Successfully.....");
			else
				printf("\n\t No such Agent to delete......");

		}	
		else if(opt == 3)
		{
			printf("\n\t Enter Train ID : ");
			scanf("%d", &train_id);
			write(csock, &train_id, sizeof(train_id));
			
			
			printf("\n\t Deleted Succesfully...........");
		}	
		else if(opt == 4)
		{
			return;
		}
		else
		{
			printf("\n\t Invalid option....");
		}
}


int agent(int csock, char uid[])
{
	int opt;
	system("clear");
	printf("\n\t 1.BookTicket \n\t 2.ViewBookings \n\t 3.UpdateBooking \n\t 4.Quit \n\t Your Response : ");
	scanf("%d", &opt);
	write(csock, &opt, sizeof(opt));
	switch(opt)
	{
		case 1: bookticket(csock, uid);
			return 1;
		case 2: viewbks(csock, uid);
			return 1;
		case 3: updatebk(csock, uid);
			return 1;
		case 4: printf("\n\t Disconnected ...........");
			exit(0);
	}
	
}
void Abookticket(int csock, char uid[])
{
	char buff[1024], status[2];
	int train_id, n, avl;
	memset(buff, '\0', sizeof(buff));
	read(csock, buff, sizeof(buff));
	printf("\n\t ID \t Train_Name \t No. of seats: ");
	printf("%s", buff);
	printf("\n\t Enter train ID: ");
	scanf("%d", &train_id);
	write(csock, &train_id, sizeof(train_id));
	read(csock, &avl, sizeof(avl));
	if(avl == 0)
	{
		printf("\n\t train/seats not available......");
		exit(0);
	}
	printf("\n\t Enter number of seats : ");
	scanf("%d", &n);
	write(csock, &n, sizeof(n));
	if(avl < n)
	{
		printf("\n Train/seats not available...");
		exit(0);
	}
	read(csock, status, sizeof(status));
	if(status[0] == 'F')
		printf("\n\t you already booked in another train ");
	else
		printf("\n\t successfully booked........");
}
void Aviewbks(int csock, char uid[])
{
	char buff[1024];
	memset(buff, '\0', sizeof(buff));
	read(csock, buff, sizeof(buff));
	printf("%s",buff);
}
void Aupdatebk(int csock, char uid[])
{
	struct bookings
        {
                char email[30];
                int train_id;
                int no_of_seats;
                int bk_status;
        }b;

	char flag[10], status[20];
	int opt, seats;
	read(csock, &flag, sizeof(flag));
	if(!strcmp(flag, "exist"))
	{
		read(csock, &b, sizeof(b));
		printf("\n\t Your bookings :\n");
		printf("\n\t %s train_ID = %d NO_seats = %d, status = %d\n", b.email, b.train_id, b.no_of_seats, b.bk_status);

		printf("\n\t 1.Increase seats \n\t 2.Decrease seats .");
		scanf("%d", &opt);
		printf("\n\t Enter NO of seats to Inc/Dec : ");
		scanf("%d",&seats );		
		
		write(csock, &opt, sizeof(opt));
		write(csock, &seats, sizeof(seats));		
		
		read(csock, status, sizeof(status));
		if(!strcmp(status, "success\\0"))
		{
			printf("\n\t SUCCESSFULY UPDATED.........");
		}				
		else
		{
			printf("\n\t Operation unsuccessful");
		}
	}
	else
	{
		printf("not exists");
	}
}

int customer(int csock, char uid[])
{
	int opt;
	//system("clear");
	printf("\n\t 1.BookTicket \n\t 2.ViewPreviousBookings \n\t 3.UpdateBooking \n\t 4.CancelBooking \n\t 5.Quit \n\t Your Response : ");
	scanf("%d", &opt);
	write(csock, &opt, sizeof(opt));
	switch(opt)
	{
		case 1: bookticket(csock, uid);
			return 1;
		case 2: viewbks(csock, uid);
			return 1;
		case 3: updatebk(csock, uid);
			return 1;
		case 4: cancelbk(csock, uid);
			return 1;
		case 5: printf("\n\t Disconnected ...........");
			exit(0);
	}
	
}

void bookticket(int csock, char uid[])
{
	char buff[1024], status[2];
	int train_id, n, avl;
	memset(buff, '\0', sizeof(buff));
	read(csock, buff, sizeof(buff));
	printf("\n\t ID \t Train_Name \t No. of seats: ");
	printf("%s", buff);
	printf("\n\t Enter train ID: ");
	scanf("%d", &train_id);
	write(csock, &train_id, sizeof(train_id));
	read(csock, &avl, sizeof(avl));
	if(avl == 0)
	{
		printf("\n\t train/seats not available......");
		return;
		//exit(0);
	}
	printf("\n\t Enter number of seats : ");
	scanf("%d", &n);
	write(csock, &n, sizeof(n));
	if(avl < n)
	{
		printf("\n Train/seats not available...");
		return;
		//exit(0);
	}
	read(csock, status, sizeof(status));
	if(status[0] == 'F')
		printf("\n\t you already booked in another train ");
	else
		printf("\n\t successfully booked........");
	return; 
}
void viewbks(int csock, char uid[])
{
	char buff[1024];
	memset(buff, '\0', sizeof(buff));
	read(csock, buff, sizeof(buff));
	printf("%s",buff);
	return;
}
void updatebk(int csock, char uid[])
{
	struct bookings
        {
                char email[30];
                int train_id;
                int no_of_seats;
                int bk_status;
        }b;

	char flag[10], status[20];
	int opt, seats;
	read(csock, &flag, sizeof(flag));
	if(!strcmp(flag, "exist"))
	{
		read(csock, &b, sizeof(b));
		printf("\n\t Your bookings :\n");
		printf("\n\t %s train_ID = %d NO_seats = %d, status = %d\n", b.email, b.train_id, b.no_of_seats, b.bk_status);

		printf("\n\t 1.Increase seats \n\t 2.Decrease seats .");
		scanf("%d", &opt);
		printf("\n\t Enter NO of seats to Inc/Dec : ");
		scanf("%d",&seats );		
		
		write(csock, &opt, sizeof(opt));
		write(csock, &seats, sizeof(seats));		
		
		read(csock, status, sizeof(status));
		if(!strcmp(status, "success\\0"))
		{
			printf("\n\t SUCCESSFULY UPDATED.........");
		}				
		else
		{
			printf("\n\t Operation unsuccessful");
		}
		return;
	}
	else
	{
		printf("not exists");
		return;
	}
	return;
}
void cancelbk(int csock, char uid[])
{
	char status[2];
	read(csock, status, sizeof(status));
	if(status[0] == 'S')
		printf("\n\t Your bookings are cancelled...");
	else
		printf("\n\t You have no bookings to cancel.....");
}









