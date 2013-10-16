#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#define MAX_PASS_LEN 8
char * getpass(const char * promat)
{
	static  char buf[MAX_PASS_LEN + 1] = {0} ;
	FILE *fp;
	sigset_t sig , osig ;
	struct termios ts , ots ;
	char *ptr ;
	int c ;

	fp = fopen(ctermid(NULL),"r+");		// open current termio

	if(fp == NULL)					
		return NULL ;

	setbuf(fp,NULL);					//set fp  no buff 

	//set signal 
	sigemptyset(&sig);
	sigaddset(&sig,SIGINT);
	sigaddset(&sig,SIGTSTP);
	sigprocmask(SIG_BLOCK,&sig,&osig);  //save mask

	//set termios 
	tcgetattr(fileno(fp),&ts);
	ots = ts ;
	ts.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	tcsetattr(fileno(fp),TCSAFLUSH,&ts );

	fputs(promat,fp);
	ptr = buf ;

	while(  (c = fgetc(fp))  != EOF &&  c != '\n')
		if(ptr < &buf[MAX_PASS_LEN ])
			*ptr ++ = c ;
	
	
	*ptr = '\0' ;
	putc('\n',fp);  //we echo a newline 


	tcsetattr(fileno(fp),TCSAFLUSH, &ots);	//restore tty state 
	sigprocmask(SIG_SETMASK,&osig , NULL);  //restore mask 
	fclose(fp);

	return buf ;
}

void err_sys(const char * buf)
{
	printf("%s.\n",buf);
}

int main()
{
	char *ptr = NULL ;

	if((ptr = getpass("Enter password:")) == NULL)
	{
		err_sys("get password err");
		exit(EXIT_FAILURE);
	}

	printf("your password : %s .\n",ptr);

	while(*ptr != '\0')
		*ptr ++ = 0 ;

	exit(EXIT_SUCCESS);
}
