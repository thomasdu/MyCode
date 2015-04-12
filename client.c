#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUF 1024

int main()
{
	int client_fd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == client_fd)
	{
		printf("socket failed, code:%d, msg:%s\n",errno,strerror(errno));
		return -1;
	}
	
	struct sockaddr_in client_addr;
	bzero(&client_addr,sizeof(client_addr));
	client_addr.sin_port = htons(10005);
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	client_addr.sin_family = AF_INET;
	
	int ret = connect(client_fd,(struct sockaddr*)&client_addr,sizeof(client_addr));
	if(-1 == ret)
	{
		printf("connect failed, code:%d, msg:%s\n",errno,strerror(errno));
		return -1;
	}
	
	//查看随机分配的端口
	socklen_t len = 0;
	ret = getsockname(client_fd,(struct sockaddr*)&client_addr,&len);
	
	if(-1 == ret)
	{
		printf("getsockname failed, code:%d, msg:%s\n",errno,strerror(errno));
		return -1;
	}

	int loop = 0;
	char read_buf[MAX_BUF];
	char send_msg[MAX_BUF];
	while(loop < 5)
	{
        loop++;		
		//sprintf(send_msg,"%d",loop);
        send_msg[0] = '\0';
		int write_len = write(client_fd,send_msg,strlen(send_msg));
		if(-1 == write_len)
		{
			printf("write failed, code:%d, msg:%s\n",errno,strerror(errno));
			return -1;
		}
		
		int read_len = read(client_fd,read_buf,MAX_BUF);
		read_buf[read_len] = 0;
		
		if(0 == read_len)
		{
			printf("i read 0 byte, code:%d, msg:%s\n",errno,strerror(errno));
			return -1;
		}
		
		printf("%s\n",read_buf);		
        sleep(5);
	}

    printf("i am here!%d\n",ret);
    close(client_fd);
	return 0;
}
