#include "header.h"

int main()
{

    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0) return 0;

    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8090);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
    {
        close(sockfd);
        return 0;
    }

    if(listen(sockfd,10) < 0)
    {
        close(sockfd);
        return 0;
    }

    int epollfd = epoll_create(1024);
    struct epoll_event events[1024];
    int fd_numbers = 1;

    struct epoll_event event;
    event.data.fd = sockfd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,sockfd,&event);
    
    while(1)
    {
        int n = epoll_wait(epollfd,events,fd_numbers,-1);
        if(n <1)
            break;
        int i =0;
        for( i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;
            short fd_event = events[i].events;

            if((fd_event & EPOLLHUP) || (fd_event & EPOLLERR) || !(fd_event &EPOLLIN))
            {
                perror("epoll error");
                close(fd);
                epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,NULL);
                --fd_numbers;
            }

            else if ( fd == sockfd)
            {
                struct sockaddr_in clientaddr;
                bzero(&clientaddr,sizeof(clientaddr));
                socklen_t len = sizeof(clientaddr);
                int client_fd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
                struct epoll_event client_event;
                client_event.data.fd = client_fd;
                client_event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,client_fd,&client_event);
                ++fd_numbers;
            }
            else
            {
                char msg[4096];
                bzero(msg,sizeof(msg));
                int recv_ret = recv(fd,msg,sizeof(msg),0);
                
                if(recv_ret == 0)
                {
                    printf("close connection with %d\n",fd);
                    close(fd);
                    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,NULL);
                    --fd_numbers;
                }
                else if(recv_ret > 0)
                {
                    printf("recv from %d:%s\n",fd,msg);
                    char reply[] = "server has received client mag";
                    if(send(fd,reply,strlen(reply),MSG_NOSIGNAL)==0)
                    {
                        printf("close connection with %d\n",fd);
                        close(fd);
                        epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,NULL);
                        --fd_numbers;
                    }
                }
            }
        }

    }

    close(epollfd);
    close(sockfd);

    return 0;
}
