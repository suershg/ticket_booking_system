#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#define PORT 8686

void login(int, char[], char[]);
void signup(int, char[], char[]);
void initiate(int);

int admin(int, char[]);
void add(int);
void modify(int);
void search(int);
void delete(int);

int agent(int, char[]);
void Abookticket(int, char[]);
void Aviewbks(int, char[]);
void Aupdatebk(int, char[]);

int n_user(int, char[]);
void bookticket(int, char[]);
void viewbks(int, char[]);
void updatebk(int, char[]);
void cancelbk(int, char[]);

int main()
{
        int sockfd, ret, opt_display;
        struct sockaddr_in servaddr;
        int newsocket;
        struct sockaddr_in newaddr;
        char buf[1024], uid[20], pwd[20];
        socklen_t addr_size;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if(sockfd < 0)
        {
                             printf("\n\t error occured");
                exit(0);
        }

        memset(&servaddr, '\0', sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
        listen(sockfd, 10);
	while(1)
	{
        	newsocket = accept(sockfd, (struct sockaddr*)&newaddr, &addr_size);
		if(fork()==0)
		{
			close(sockfd);
			initiate(newsocket);
			exit(0);        	
		}
	}
        close(newsocket);
        return 0;
}
     
void initiate(int newsocket)
{
	int opt;
	char uid[30], pwd[30];
	read(newsocket, &opt, sizeof(opt));
	read(newsocket, uid, sizeof(uid));
	read(newsocket, pwd, sizeof(pwd));
	if(opt == 1)
	{
		login(newsocket,uid, pwd);
	}
	else if(opt == 2)
	{
		signup(newsocket,uid, pwd);
	}
	else
	{
		printf("\n\t Disconnected : ");
		exit(0);
	}
}

void login(int newsocket, char uid[], char pwd[])
{
	int fd1, res, type;
	struct user
	{
		char email[30];
		char pwd[30];
		int type;
	}usr;
        
	struct flock lock;
	lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
	
	fd1 = open("user_db", O_RDWR);
	
	while(read(fd1, &usr,sizeof(usr)))
	{
		printf("\t\t %s  %s\n", usr.email, usr.pwd);
		if((!strcmp(usr.email, uid)) && (!strcmp(usr.pwd, pwd)))
		{	
			lock.l_start = lseek(fd1, -sizeof(usr), SEEK_CUR);
			lock.l_len = sizeof(usr);

			printf("\n\t %s %s", uid, pwd);
			type = usr.type;
			if(usr.type == 1)
			{
				res = fcntl(fd1, F_SETLK, &lock);
				if(res == -1)
				{
					type = res;
				}
				else
				{
					write(newsocket, &type, sizeof(type));
					while(admin(newsocket, uid));
				}
			}
			else if(usr.type == 2)
			{
				lock.l_type = F_RDLCK;
				res = fcntl(fd1, F_SETLK, &lock);
				if(res == -1)
				{
					type = res;
				}
				else
				{
					write(newsocket, &type, sizeof(type));
					while(agent(newsocket, uid));
				}
			}
			else if(usr.type == 3)
			{
				res = fcntl(fd1, F_SETLK, &lock);
				if(res == -1)
				{
					type = res;
				}
				else
				{
					write(newsocket, &type, sizeof(type));
					while(n_user(newsocket, uid));
				}
			}
			if(type == -1)
			{
				write(newsocket, &type, sizeof(type));
			}
			lock.l_type = F_UNLCK;
			fcntl(fd1, F_SETLK, &lock);
			return;
		}
		
	}
	type = -1;
	printf("\n\t Invalid Credentials ......... \n");
	write(newsocket, &type, sizeof(type));
	return;
}



void signup(int newsocket, char uid[], char pwd[])
{
        int fd1 = open("user_db", O_RDWR), flag=0;
        struct user
        {
                char email[30];
                char pwd[30];
                int type;
        }customer;
        while(read(fd1,&customer,sizeof(customer)) != 0)
	{
		if(!strcmp(customer.email, uid))
		{
			flag = -1;
		}
	}
	if(flag == -1)
	{
		printf("\n\t Already Exists...........\n");
		write(newsocket, &flag, sizeof(flag));
	}
	else
	{
		strcpy(customer.email, uid);
		strcpy(customer.pwd, pwd);
		customer.type = 3;
		write(fd1, &customer, sizeof(customer));
	}
	write(newsocket, &customer.type, sizeof(customer.type));
	
	n_user(newsocket, uid);

        lseek(fd1, 0, SEEK_SET);
	while(read(fd1,&customer,sizeof(customer)))
	{
		printf("\n\t %s   %s   %d   \n ", customer.email, customer.pwd, customer.type);
	}
        
}



int admin(int csock, char uid[])
{
	int opt;
	//printf("\n\t 1.Add \n\t 2.Modify \n\t 3.Search \n\t 4.Delete \n\t 5.Quit");
	read(csock, &opt, sizeof(opt));
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
		case 5: exit(0);
	}
	
}
void add(int csock)
{
	int opt, fd1, fd2, fd3;
	printf("\n\t add called");
	//printf("\n\t 1.USER \n\t 2.AGENT \n\t 3.TRAIN");
	//read(csock, &opt, sizeof(opt));
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
	}t;
	
		read(csock, &opt, sizeof(opt));
		if(opt == 1)
		{
			read(csock, u.email, sizeof(u.email));
			read(csock, u.pwd, sizeof(u.pwd));
			u.type = 3;
			fd1 = open("user_db", O_RDWR);
			lseek(fd1, 0, SEEK_END);
			write(fd1, &u, sizeof(u));
					
		}
		else if(opt == 2)
		{
			struct user u1;
			read(csock, u1.email, sizeof(u1.email));
			read(csock, u1.pwd, sizeof(u1.pwd));
			u1.type = 2;
			fd1 = open("user_db", O_RDWR);
			lseek(fd1, 0, SEEK_END);
			write(fd1, &u1, sizeof(u1));
		}
		else if(opt == 3)
		{
			read(csock, &t.train_id, sizeof(t.train_id));
			read(csock, t.train, sizeof(t.train));
			read(csock, &t.seats_avl, sizeof(t.seats_avl));
		
			fd1 = open("train_db", O_RDWR);
			lseek(fd1, 0, SEEK_END);
			write(fd1, &t, sizeof(t));
		}
		else if(opt == 4)
		{
			return;
		}
		else
		{
			printf("\n\t INVALID OPTION........");
			return;
		}
		return;
}

void modify(int csock)
{
	printf("\n\t modify called");
	int opt, fd1, fd2, fd3;
	printf("\n\t add called");
	//printf("\n\t 1.USER \n\t 2.AGENT \n\t 3.TRAIN");
	//read(csock, &opt, sizeof(opt));
	struct user
	{
		char email[30];
		char pwd[30];
		int type;
	};
	struct user u, u1;
	struct train
	{
		int train_id;
		char train[20];
		int seats_avl;
	};
	struct train t, t1;
	struct flock lock;
	
		read(csock, &opt, sizeof(opt));
		if(opt == 1)
		{
			read(csock, u.email, sizeof(u.email));
			read(csock, u.pwd, sizeof(u.pwd));
			u.type = 3;
			fd1 = open("user_db", O_RDWR);
			while(read(fd1, &u1, sizeof(u1)))
			{
				if(!(strcmp(u1.email, u.email))  && u1.type == 3)
				{
					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_SET;
					lock.l_start = lseek(fd1, -sizeof(u), SEEK_CUR);
					lock.l_len = sizeof(u);
					fcntl(fd1, F_SETLKW, &lock);
					write(fd1, &u, sizeof(u));
					lock.l_type = F_UNLCK;
					fcntl(fd1, F_SETLK, &lock);
					return;
				}
			
			}
			lseek(fd1, 0, SEEK_END);
			write(fd1, &u, sizeof(u));
		
		}
		else if(opt == 2)
		{
			read(csock, u.email, sizeof(u.email));
			read(csock, u.pwd, sizeof(u.pwd));
			u.type = 2;
			fd1 = open("user_db", O_RDWR);
			while(read(fd1, &u1, sizeof(u1)))
			{
				if(!(strcmp(u1.email, u.email))  && u1.type == 2)
				{
					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_SET;
					lock.l_start = lseek(fd1, -sizeof(u), SEEK_CUR);
					lock.l_len = sizeof(u);
					fcntl(fd1, F_SETLKW, &lock);
					write(fd1, &u, sizeof(u));
					lock.l_type = F_UNLCK;
					fcntl(fd1, F_SETLK, &lock);
					return;
				}
			
			}
			lseek(fd1, 0, SEEK_END);
			write(fd1, &u, sizeof(u));
		
		}
		else if(opt == 3)
		{
			char status;
			read(csock, &t.train_id, sizeof(t.train_id));
			read(csock, &t.seats_avl, sizeof(t.seats_avl));
		
			fd1 = open("train_db", O_RDWR);
			while(read(fd1, &t1, sizeof(t1)))
			{
				if(t1.train_id == t.train_id)
				{
					status = 'S';
					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_SET;
					lock.l_start = lseek(fd1, -sizeof(t), SEEK_CUR);
					lock.l_len = sizeof(t);
					fcntl(fd1, F_SETLKW, &lock);
					
					t.seats_avl = t.seats_avl + t1.seats_avl;
					if(t.seats_avl < 0)
						t.seats_avl = 0;
					strcpy(t.train, t1.train);

					write(fd1, &t, sizeof(t));
					lock.l_type = F_UNLCK;
					fcntl(fd1, F_SETLK, &lock);
					write(csock, &status, sizeof(status));
					return;
				}
			}
			status = 'F';
			write(csock, &status, sizeof(status));
			
		}
		else if(opt == 4)
		{
			return;
		}
		else
		{
			printf("\n\t INVALID OPTION........");
			return;
		}
	
}
void search(int csock)
{
	int opt,fd1;
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
	read(csock, &opt, sizeof(opt));
	switch(opt)
	{
		case 1: read(csock, uid, sizeof(uid));
			fd1 = open("user_db", O_RDWR);
			while(read(fd1, &u, sizeof(u)))
			{
				if(!strcmp(uid, u.email) && u.type == 3)
				{
					status = 'S';
					write(csock, &status, sizeof(status));
					write(csock, &u, sizeof(u));
					return;
				}
			}
			status = 'F';
			write(csock, &status, sizeof(status));
			break;
		case 2: read(csock, uid, sizeof(uid));
			fd1 = open("user_db", O_RDWR);
			while(read(fd1, &u, sizeof(u)))
			{
				if(!strcmp(uid, u.email) && u.type == 2)
				{
					status = 'S';
					write(csock, &status, sizeof(status));
					write(csock, &u, sizeof(u));
					return;
				}
			}
			status = 'F';
			write(csock, &status, sizeof(status));
			break;
		case 3: read(csock, uid, sizeof(uid));
			fd1 = open("train_db", O_RDWR);
			while(read(fd1, &t, sizeof(t)))
			{
				if(!strcmp(uid, t.train))
				{
					status = 'S';
					write(csock, &status, sizeof(status));
					write(csock, &t, sizeof(t));
					return;
				}
			}
			status = 'F';
			write(csock, &status, sizeof(status));
			break;
		default : return;
	}
	
}
void delete(int csock)
{
	int opt, fd1, fd2, fd3;
	char status;
	printf("\n\t add called");
	//printf("\n\t 1.USER \n\t 2.AGENT \n\t 3.TRAIN");
	//read(csock, &opt, sizeof(opt));
	struct user
	{
		char email[30];
		char pwd[30];
		int type;
	};
	struct user u, u1;
	struct train
	{
		int train_id;
		char train[20];
		int seats_avl;
	}t;
	struct bookings
	{
		char email[30];
		int train_id;
		int no_of_seats;
		int bk_status;
	}b;
	struct flock lock;
		read(csock, &opt, sizeof(opt));
		if(opt == 1)
		{
			read(csock, u.email, sizeof(u.email));
			fd1 = open("user_db", O_RDWR);
			while(read(fd1, &u1, sizeof(u1)))
			{
				if(!strcmp(u1.email, u.email) && u1.type == 3)
				{
					status = 'S';
					strcpy(u1.email, "Deleted\\0");

					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_SET;
					lock.l_start = lseek(fd1, -sizeof(u1), SEEK_CUR);
					lock.l_len = sizeof(u1);
					fcntl(fd1, F_SETLKW, &lock);
					write(fd1, &u1, sizeof(u1));
					lock.l_type = F_UNLCK;
					fcntl(fd1, F_SETLK, &lock);
					
					fd2 = open("bookings", O_RDWR);
					while(read(fd2, &b, sizeof(b)))
					{
						if(!strcmp(b.email, u.email))
						{
							strcpy(b.email, "Deleted\\0");
							lseek(fd2, -sizeof(b), SEEK_CUR);
							write(fd2, &b, sizeof(b));
							
							fd3 = open("train_db", O_RDWR);
							while(read(fd3, &t, sizeof(t)))
							{
								if(t.train_id == b.train_id)
								{
									
									t.seats_avl = t.seats_avl + b.no_of_seats;
									lock.l_type = F_WRLCK;
									lock.l_whence = SEEK_SET;
									lock.l_start = lseek(fd3, -sizeof(t), SEEK_CUR);
									lock.l_len = sizeof(t);
									fcntl(fd3, F_SETLKW, &lock);
									write(fd3, &t, sizeof(t));
									lock.l_type = F_UNLCK;
									fcntl(fd3, F_SETLK, &lock);
								}	
							}
						}
					}
					write(csock, &status, sizeof(status));	
					return;
				}
			}
			status = 'F';
			write(csock, &status, sizeof(status));
		
		}
		else if(opt == 2)
		{
			read(csock, u.email, sizeof(u.email));
			fd1 = open("user_db", O_RDWR);
			while(read(fd1, &u1, sizeof(u1)))
			{
				if(!strcmp(u1.email, u.email) && u1.type == 2)
				{
					status = 'S';
					strcpy(u1.email, "Deleted\\0");

					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_SET;
					lock.l_start = lseek(fd1, -sizeof(u1), SEEK_CUR);
					lock.l_len = sizeof(u1);
					fcntl(fd1, F_SETLKW, &lock);
					write(fd1, &u1, sizeof(u1));
					lock.l_type = F_UNLCK;
					fcntl(fd1, F_SETLK, &lock);
					
					fd2 = open("bookings", O_RDWR);
					while(read(fd2, &b, sizeof(b)))
					{
						if(!strcmp(b.email, u.email))
						{
							strcpy(b.email, "Deleted\\0");
							lseek(fd2, -sizeof(b), SEEK_CUR);
							write(fd2, &b, sizeof(b));
							
							fd3 = open("train_db", O_RDWR);
							while(read(fd3, &t, sizeof(t)))
							{
								if(t.train_id == b.train_id)
								{
									t.seats_avl = t.seats_avl + b.no_of_seats;
									lock.l_type = F_WRLCK;
									lock.l_whence = SEEK_SET;
									lock.l_start = lseek(fd3, -sizeof(t), SEEK_CUR);
									lock.l_len = sizeof(t);
									fcntl(fd3, F_SETLKW, &lock);
									write(fd3, &t, sizeof(t));
									lock.l_type = F_UNLCK;
									fcntl(fd3, F_SETLK, &lock);
								}	
							}
						}
					}
					write(csock, &status, sizeof(status));	
					return;
				}
			}
			status = 'F';
			write(csock, &status, sizeof(status));
		}
		else if(opt == 3)
		{
			int train_id;
			read(csock, &train_id, sizeof(train_id));
			fd1 = open("train_db", O_RDWR);
			while(read(fd1,&t, sizeof(t)))
			{
				if(train_id == t.train_id)
				{
					t.train_id = 0;
					strcpy(t.train, "Deleted\\0");
					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_SET;
					lock.l_start = lseek(fd1, -sizeof(t), SEEK_CUR);
					lock.l_len = sizeof(t);
					fcntl(fd1, F_SETLKW, &lock);
					write(fd1, &t, sizeof(t));
					lock.l_type = F_UNLCK;
					fcntl(fd1, F_SETLK, &lock);
					
					fd2 = open("bookings", O_RDWR);
					while(read(fd2, &b, sizeof(b)))
					{
						if(train_id == b.train_id)
						{
							b.no_of_seats = 0;
							strcpy(b.email, "Deleted");
							lock.l_type = F_WRLCK;
							lock.l_whence = SEEK_SET;
							lock.l_start = lseek(fd2, -sizeof(b), SEEK_CUR);
							lock.l_len = sizeof(b);
							fcntl(fd2, F_SETLKW, &lock);
							write(fd2, &b, sizeof(b));
							lock.l_type = F_UNLCK;
							fcntl(fd2, F_SETLK, &lock);
						}
					}	
				}
			}
			
		}
		else if(opt == 4)
		{
			return;
		}
		else
		{
			printf("\n\t INVALID OPTION........");
			return;
		}
		return;
}




int agent(int csock, char uid[])
{
	int opt;
	//printf("\n\t 1.BookTicket \n\t 2.ViewPreviousBookings \n\t 3.UpdateBooking \n\t Your Response : ");
	read(csock, &opt, sizeof(opt));
	switch(opt)
	{
		case 1: Abookticket(csock, uid);
			return 1;
		case 2: Aviewbks(csock, uid);
			return 1;
		case 3: Aupdatebk(csock, uid);
			return 1;
		case 4: printf("\n\t Disconnected ...........");
			exit(0);
	}
	
}

void Abookticket(int csock, char uid[])
{
	char buff[1024], idbuff[10], seatbuff[10], sbuff[50], status[2] = "S";
	int fd, fd1, train_id, n; 
	struct flock lock;	
	struct train
	{
		int train_id;
		char train[20];
		int seats_avl;
	}t;
	
	struct bookings
	{
		char email[30];
		int train_id;
		int no_of_seats;
		int bk_status;
	};
	struct bookings b, b1;
	
	fd = open("train_db", O_RDWR);
	printf("\n\t successfully executed    ");
	memset(buff, '\0', sizeof(buff));
	while(read(fd, &t, sizeof(t)))
	{   
	   if(t.train_id != 0)
	   {
		strcat(buff,"\n\t");
		sprintf(idbuff,"%4d",t.train_id);
		strcat(buff,idbuff);
		//strcat(buff,"---");
		sprintf(sbuff, "%15s", t.train);
		strcat(buff, sbuff);
               // strcat(buff,"---");
		sprintf(seatbuff,"%9d",t.seats_avl);
		strcat(buff,seatbuff);
                //strcat(buff,"\n");
           }

	}
	write(csock, buff, sizeof(buff));
	read(csock, &train_id, sizeof(train_id));
	printf("\n\ttrainid = %d", train_id);
	lseek(fd, 0, SEEK_SET);
	
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	
	while(read(fd, &t, sizeof(t)))
	{
		if(t.train_id == train_id)
		{
			lock.l_start = lseek(fd, -1 * sizeof(t), SEEK_CUR);
			lock.l_len = sizeof(t);
			fcntl(fd, F_SETLKW, &lock);
			write(csock, &t.seats_avl, sizeof(t.seats_avl));
			if(t.seats_avl == 0)
			{
				return;
			}
			read(csock, &n, sizeof(n));
			if(t.seats_avl < n)
			{
				return;
			}
			
			fd1 = open("bookings", O_RDWR);

			strcpy(b.email, uid);
			b.train_id = train_id;
			b.no_of_seats = n;
			b.bk_status = 1;
			
			while(read(fd1, &b1, sizeof(b1)))
			{
				if(!strcmp(b1.email, uid))
				{
					if(b1.train_id != b.train_id)
					{
							status[0] = 'F'; status[1] = '\0';
							write(csock, status, sizeof(status));
							return;
					}
					b.no_of_seats = n + b1.no_of_seats;
					break;
				}
			}
			if(strcmp(b1.email, uid))
			{
				printf("\n\t one");
				lseek(fd1,0, SEEK_END);			
				write(fd1, &b, sizeof(b));
			}
			else
			{
				printf("\n\t two ");
				
					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_SET;
					lock.l_start = lseek(fd1, -sizeof(b), SEEK_CUR);
					lock.l_len = sizeof(b);
					fcntl(fd1, F_SETLKW, &lock);

					write(fd1, &b, sizeof(b));
					lock.l_type = F_UNLCK;
					fcntl(fd1, F_SETLK, &lock);

				/*lseek(fd1, -sizeof(b), SEEK_CUR);
				write(fd1, &b, sizeof(b));	*/
			}

			t.seats_avl = t.seats_avl - n;
			write(fd, &t, sizeof(t));

			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
			write(csock, status, sizeof(status));
			return;
		}
	}
	n = 0;
	write(csock, &n, sizeof(n));
	return;
}
void Aviewbks(int csock, char uid[])
{
	int fd;
	char buf1[10], buf2[10], buf3[10], buff[1024];
	struct bookings
        {
                char email[30];
                int train_id;
                int no_of_seats;
                int bk_status;
        }b;
	struct flock lock;
	
	fd = open("bookings", O_RDWR);

	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = lseek(fd, -sizeof(b), SEEK_CUR);
	lock.l_len = sizeof(b);
	fcntl(fd, F_SETLKW, &lock);
	
	memset(buff, '\0', sizeof(buff));
	while(read(fd, &b, sizeof(b)))
	{
		if(!strcmp(uid, b.email))
		{
			printf("\n\t %s--%d--%d--%d", b.email, b.train_id, b.no_of_seats, b.bk_status);
			strcat(buff,"\n\t");
			strcat(buff, b.email);
			strcat(buff,"---");
			sprintf(buf1,"%d",b.train_id);
			strcat(buff,buf1);
			strcat(buff,"---");
			sprintf(buf2,"%d",b.no_of_seats);
			strcat(buff,buf2);
			strcat(buff,"---");
			sprintf(buf3,"%d",b.bk_status);
			strcat(buff,buf3);
			break;
		}
	}
	write(csock, buff, sizeof(buff));
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	return;
	printf("\n\t successfully executed");
}
void Aupdatebk(int csock, char uid[])
{
	char flag[10] = "exist", status[20] = "Operation Failed";

	int fd, opt, seats, fd1;
	struct bookings
        {
                char email[30];
                int train_id;
                int no_of_seats;
                int bk_status;
        }b;
	
	struct train
	{
		int train_id;
		char train[20];
		int seats_avl;
	}t;
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;

	fd = open("bookings", O_RDWR);
	fd1 = open("train_db", O_RDWR);

	while(read(fd, &b, sizeof(b)))
	{
		if(!strcmp(b.email, uid))
		{
			write(csock, flag, sizeof(flag));
			write(csock, &b, sizeof(b));			

			read(csock, &opt, sizeof(opt));
			read(csock, &seats, sizeof(seats));			
			
			while(read(fd1, &t, sizeof(t)))
			{
				if(b.train_id == t.train_id)
				{
					break;
				}
			}
			if(opt == 1)
			{
				if(t.seats_avl >= seats)
				{
					strcpy(status,"success\\0");

					lock.l_start = lseek(fd, -sizeof(b), SEEK_CUR);
					lock.l_len = sizeof(b);
					b.no_of_seats = b.no_of_seats + seats;
					fcntl(fd, F_SETLKW, &lock);
					write(fd, &b, sizeof(b));
					lock.l_type = F_UNLCK;
					fcntl(fd, F_SETLK, &lock);

					t.seats_avl = t.seats_avl - seats;
					lock.l_start = lseek(fd1, -sizeof(t), SEEK_CUR);
					lock.l_len = sizeof(t);
					fcntl(fd1, F_SETLKW, &lock);
					write(fd1, &t, sizeof(t));
					lock.l_type = F_UNLCK;
					fcntl(fd1, F_SETLKW, &lock);
				}
			}
			else if(opt == 2)
			{
				strcpy(status,"success\\0");
				if(b.no_of_seats >= seats)
				{
					t.seats_avl = t.seats_avl + seats;
					b.no_of_seats = b.no_of_seats - seats;
				}
				else
				{
					t.seats_avl = t.seats_avl + b.no_of_seats;
					b.no_of_seats = 0;
				}

				lock.l_start = lseek(fd, -sizeof(b), SEEK_CUR);
				lock.l_len = sizeof(b);
				fcntl(fd, F_SETLKW, &lock);

				write(fd, &b, sizeof(b));

				lock.l_type = F_UNLCK;
				fcntl(fd, F_SETLK, &lock);


				lock.l_start = lseek(fd1, -sizeof(t), SEEK_CUR);
				lock.l_len = sizeof(t);
				fcntl(fd1, F_SETLKW, &lock);

				write(fd1, &t, sizeof(t));

				lock.l_type = F_UNLCK;
				fcntl(fd1, F_SETLKW, &lock);
				
			}
			write(csock, status, sizeof(status));
			return;
		}
	}
	strcpy(flag, "notexist");
	write(csock, flag, sizeof(flag));
	printf("\n\tsuccessfully executed");
	return;
}


int n_user(int csock, char uid[])
{
	int opt;
	//printf("\n\t 1.BookTicket \n\t 2.ViewPreviousBookings \n\t 3.UpdateBooking \n\t 4.CancelBooking \n\t Your Response : ");
	read(csock, &opt, sizeof(opt));
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
	char buff[1024], idbuff[10], seatbuff[10], sbuff[50], status[2] = "S";
	int fd, fd1, train_id, n; 
	struct flock lock;	
	struct train
	{
		int train_id;
		char train[20];
		int seats_avl;
	}t;
	
	struct bookings
	{
		char email[30];
		int train_id;
		int no_of_seats;
		int bk_status;
	};
	struct bookings b, b1;
	
	fd = open("train_db", O_RDWR);
	memset(buff, '\0', sizeof(buff));
	while(read(fd, &t, sizeof(t)))
	{
           if(t.train_id != 0)
	    {
		strcat(buff,"\n\t");
		sprintf(idbuff,"%4d",t.train_id);
		strcat(buff,idbuff);
		//strcat(buff,"---");
		sprintf(sbuff, "%15s", t.train);
		strcat(buff, sbuff);
               // strcat(buff,"---");
		sprintf(seatbuff,"%9d",t.seats_avl);
		strcat(buff,seatbuff);
                //strcat(buff,"\n");
	     }

	}
	write(csock, buff, sizeof(buff));
	read(csock, &train_id, sizeof(train_id));
	printf("\n\ttrainid = %d", train_id);
	lseek(fd, 0, SEEK_SET);
	
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	
	while(read(fd, &t, sizeof(t)))
	{
		if(t.train_id == train_id)
		{
			
			write(csock, &t.seats_avl, sizeof(t.seats_avl));
			if(t.seats_avl == 0)
			{
				return;
			}
			read(csock, &n, sizeof(n));
			if(t.seats_avl < n)
			{
				return;
			}
			
			fd1 = open("bookings", O_RDWR);

			strcpy(b.email, uid);
			b.train_id = train_id;
			b.no_of_seats = n;
			b.bk_status = 1;
			
			while(read(fd1, &b1, sizeof(b1)))
			{
				if(!strcmp(b1.email, uid))
				{
					if(b1.train_id != b.train_id)
					{
							status[0] = 'F'; status[1] = '\0';
							write(csock, status, sizeof(status));
							return;
					}
					b.no_of_seats = n + b1.no_of_seats;
					break;
				}
			}
			if(strcmp(b1.email, uid))
			{
				printf("\n\t one");
				lseek(fd1,0, SEEK_END);			
				write(fd1, &b, sizeof(b));
			}
			else
			{
				printf("\n\t two ");
				lseek(fd1, -sizeof(b), SEEK_CUR);
				write(fd1, &b, sizeof(b));	
			}

			t.seats_avl = t.seats_avl - n;
			
			lock.l_start = lseek(fd, -1 * sizeof(t), SEEK_CUR);
			lock.l_len = sizeof(t);
			fcntl(fd, F_SETLKW, &lock);
			
			write(fd, &t, sizeof(t));

			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
			write(csock, status, sizeof(status));
			return;
		}
	}
	n = 0;
	write(csock, &n, sizeof(n));
	return;
}
void viewbks(int csock, char uid[])
{
	int fd;
	char buf1[10], buf2[10], buf3[10], buff[1024];
	struct bookings
        {
                char email[30];
                int train_id;
                int no_of_seats;
                int bk_status;
        }b;
	
	fd = open("bookings", O_RDWR);
	memset(buff,'\0', sizeof(buff));
	while(read(fd, &b, sizeof(b)))
	{
		if(!strcmp(uid, b.email))
		{
			printf("\n\t %s--%d--%d--%d", b.email, b.train_id, b.no_of_seats, b.bk_status);
			strcat(buff,"\n\t");
			strcat(buff, b.email);
			strcat(buff,"---");
			sprintf(buf1,"%d",b.train_id);
			strcat(buff,buf1);
			strcat(buff,"---");
			sprintf(buf2,"%d",b.no_of_seats);
			strcat(buff,buf2);
			strcat(buff,"---");
			sprintf(buf3,"%d",b.bk_status);
			strcat(buff,buf3);
			break;
		}
	}
	write(csock, buff, sizeof(buff));
	printf("\n\t successfully executed");
	return;
}
void updatebk(int csock, char uid[])
{
	char flag[10] = "exist", status[20] = "Operation Failed";

	int fd, opt, seats, fd1;
	struct bookings
        {
                char email[30];
                int train_id;
                int no_of_seats;
                int bk_status;
        }b;
	
	struct train
	{
		int train_id;
		char train[20];
		int seats_avl;
	}t;
	
	fd = open("bookings", O_RDWR);
	fd1 = open("train_db", O_RDWR);

	while(read(fd, &b, sizeof(b)))
	{
		if(!strcmp(b.email, uid))
		{
			write(csock, flag, sizeof(flag));
			write(csock, &b, sizeof(b));			

			read(csock, &opt, sizeof(opt));
			read(csock, &seats, sizeof(seats));			
			
			while(read(fd1, &t, sizeof(t)))
			{
				if(b.train_id == t.train_id)
				{
					break;
				}
			}
			if(opt == 1)
			{
				if(t.seats_avl >= seats)
				{
					strcpy(status,"success\\0");
					lseek(fd, -sizeof(b), SEEK_CUR);
					lseek(fd1, -sizeof(t), SEEK_CUR);
					t.seats_avl = t.seats_avl - seats;
					b.no_of_seats = b.no_of_seats + seats;
					write(fd, &b, sizeof(b));
					write(fd1, &t, sizeof(t));
				}
			}
			else if(opt == 2)
			{
				strcpy(status,"success\\0");
				if(b.no_of_seats >= seats)
				{
					t.seats_avl = t.seats_avl + seats;
					b.no_of_seats = b.no_of_seats - seats;
				}
				else
				{
					t.seats_avl = t.seats_avl + b.no_of_seats;
					b.no_of_seats = 0;
				}
				lseek(fd, -sizeof(b), SEEK_CUR);
				lseek(fd1, -sizeof(t), SEEK_CUR);
				write(fd, &b, sizeof(b));
				write(fd1, &t, sizeof(t));
			}
			write(csock, status, sizeof(status));
			return;
		}
	}
	strcpy(flag, "notexist");
	write(csock, flag, sizeof(flag));
	printf("\n\tsuccessfully executed");
}
void cancelbk(int csock, char uid[])
{
	int fd1, fd2;
	char status[2] = "S";
	struct flock lock; 
	struct bookings
        {
                char email[30];
                int train_id;
                int no_of_seats;
                int bk_status;
        }b;
	
	struct train
	{
		int train_id;
		char train[20];
		int seats_avl;
	}t;
	
	fd1 = open("train_db", O_RDWR);
	fd2 = open("bookings", O_RDWR);
	
	while(read(fd2, &b, sizeof(b)))
	{
		if((!strcmp(b.email, uid)) && b.no_of_seats > 0)
		{
			
			while(read(fd1, &t, sizeof(t)))
			{
				if(t.train_id == b.train_id)
				{
					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_SET;
					lock.l_start = lseek(fd1, -sizeof(t), SEEK_CUR);
					lock.l_len = sizeof(t);
					fcntl(fd1, F_SETLKW, &lock);
					t.seats_avl = t.seats_avl + b.no_of_seats;
					write(fd1, &t, sizeof(t));
					lock.l_type = F_UNLCK;
					fcntl(fd1, F_SETLK, &lock);
				}	
			}
			lseek(fd2, -sizeof(b), SEEK_CUR);
			b.no_of_seats = 0;
			write(fd2, &b, sizeof(b));
			
			write(csock, status, sizeof(status));
			return;
		}
	}
	status[0] = 'F';
	write(csock, status, sizeof(status));
	//printf("\n\tsuccessfully executed");
}





