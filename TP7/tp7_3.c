#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
void error(char *msg){
    perror(msg);
    exit(1);
}
int main(void){
    fd_set rset;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    struct sockaddr_in serv_addr, cli_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    int portno = 8888;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    int clilen = sizeof(cli_addr);

    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    error("ERROR on accept");
    //Remplacer à partir d’ici
    if(dup2(sockfd, STDIN_FILENO) < 0){
        printf("Unable to duplicate file descriptor.");
        exit(EXIT_FAILURE);
  }
    FD_ZERO(&rset);


    char buffer[256];
    bzero(buffer,256);
    int n = read(newsockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    char dest = buffer;
    while(*dest){
        *dest= toupper( *dest);
        dest++;
    }


    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
    return 0;
}