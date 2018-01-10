#include "header.h"

int main()
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in sockaddr;
    bzero(&sockaddr,sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(8090);
    sockaddr.sin_addr.s_addr =inet_addr("127.0.0.1");

    if(connect(sockfd,(struct sockaddr *) &sockaddr,sizeof(sockaddr)) < 0)
    {
        close(sockfd);
        return 0;
    }

    char msg[4096];
    while(1)
    {
        bzero(msg,sizeof(msg));
        scanf("%s",msg);
        
        if(strcmp(msg,"exit") == 0)
        {
            break;
        }

        send(sockfd,msg,strlen(msg),MSG_NOSIGNAL);
        int len = recv(sockfd,msg,sizeof(msg),0);
        msg[len] = '\0';

        printf("receive from server %s\n",msg);
    }

    close(sockfd);

    return 0;
}
