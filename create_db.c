#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<unistd.h>
int main()
{
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
		int no_avail_seats;
		//int last_seatNo;
	}t;
	struct bookings
	{
		char email[30];
		int train_id;
		int no_of_seats;
		int bk_status;
	}b;
	int fd1, fd2, fd3;

	fd1 = open("user_db", O_RDWR|O_CREAT, 0766);

	strcpy(u.email, "admin@iiib.in");
	strcpy(u.pwd, "1234");
	u.type = 1;

	write(fd1, &u, sizeof(u));

	fd2 = open("train_db", O_RDWR|O_CREAT, 0766);

	t.train_id = 1721;
	strcpy(t.train, "rajdhani");
	t.no_avail_seats = 20;

	write(fd2, &t, sizeof(t));

	fd3 = open("bookings", O_RDWR|O_CREAT, 0766);

	strcpy(b.email, "admin@iiitb.org");
	b.train_id = 1721;
	b.no_of_seats = 5;
	b.bk_status = 1;

	write(fd3, &b, sizeof(b));
	
	lseek(fd1, 0, SEEK_SET);
	lseek(fd2, 0, SEEK_SET);
	lseek(fd3, 0, SEEK_SET);
	
	while(read(fd1, &u, sizeof(u)))
	{
		printf("\n %s %s %d", u.email, u.pwd, u.type);
	}
	while(read(fd2, &t, sizeof(t)))
        {
                printf("\n %d %s %d", t.train_id, t.train, t.no_avail_seats);
        }
	while(read(fd3, &b, sizeof(b)))
        {
                printf("\n  %s %d %d %d", b.email, b.train_id, b.no_of_seats, b.bk_status);
        }


	close(fd1);
	close(fd2);
	close(fd3);
	
	return 0;
}



