#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUF 1024
#define SERVER_PORT 10005

int main()
{
	int server_fd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == server_fd)
	{
		printf("socket failed, code:%d, msg:%s\n",errno,strerror(errno));
		return -1;
	}
	
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	
	int ret = bind(server_fd,(struct sockaddr*)(&addr),sizeof(struct sockaddr));
	if(-1 == ret)
	{
		printf("bind failed, code:%d, msg:%s\n",errno,strerror(errno));
		return -1;
	}

	
	ret = listen(server_fd,10);
	if(-1 == ret)
	{
		printf("listen failed, code:%d, msg:%s\n",errno,strerror(errno));
		return -1;
	}
	
	int client_fd = -1;
	struct sockaddr_in client_addr;
	bzero(&client_addr,sizeof(client_addr));
	char read_buf[MAX_BUF];
	char send_buf[MAX_BUF];
    printf("wait connection at port:%d, pid:%d\n",SERVER_PORT,getpid());
	while(1)
	{
		socklen_t addr_size = sizeof(client_addr);
		if(-1 == (client_fd = accept(server_fd, (struct sockaddr*)&client_addr,&addr_size)))
		{
			printf("accept failed, code:%d, msg:%s\n",errno,strerror(errno));
			return -1;
		}
		
		printf("receive client request, ip:%s, port:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		
		//建立连接后fork新进程处理
		int pid = fork();		
		if(pid == 0)
		{
			//子进程
			close(server_fd);
        	printf("kernel alloc resource, ip:%s, port:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			while(1)
			{
				int read_len = read(client_fd,read_buf,MAX_BUF);
				if(0 == read_len)
				{
                    printf(" i read 0 count byte! errno=%d, error=%s\n", errno, strerror(errno));
					sleep(1);
					continue;
				}
                else if (-1 == read_len)
                {
                    fprintf(stderr, "read error, error=%d, error=%s\n", errno, strerror(errno));
                    break;
                }
				read_buf[read_len] = 0;
				printf("receive msg: %s\n",read_buf);
				
				sprintf(send_buf,"ack %s;",read_buf);
				write(client_fd,send_buf,strlen(send_buf));
			}
		}
		else if(pid > 0)
		{
			printf("child pid:%d\n",pid);
			close(client_fd);
			continue;
		}
		else if(pid < 0)
		{
			printf("fork failed, code:%d, msg:%s\n",errno,strerror(errno));
			return -1;
		}		
		
	}

}

