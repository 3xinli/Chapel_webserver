#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void webserver()
{
    int portNumber = 8000;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr, cliaddr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(portNumber);
    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr))<0)
    {
        printf("Error binding. If you just stopped this server, wait a few seconds.\n");
        return;
    }
    listen(sockfd, 5);
    socklen_t clilen = sizeof(cliaddr);
    /* Write Header */
    char replyHeader[128];
    memset(replyHeader, 0, sizeof(replyHeader));
    strcat(replyHeader, "HTTP/1.0 200 OK\r\n");
    strcat(replyHeader, "Server: Xin's Chapel web Server\r\n");
    char curtime[32];
    time_t t = time(NULL);
    strftime(curtime, 32, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    strcat(replyHeader, "Date: ");
    strcat(replyHeader, curtime);
    strcat(replyHeader, "\r\n");
    strcat(replyHeader, "Content-type: text/html\r\n");

    while (1)
    {
        int newsockfd = accept(sockfd, (struct sockaddr*) &cliaddr, &clilen);
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        read(newsockfd,buffer,1023);
        /* buffer is not used here. We assume this is a simple and legitimate HTTP request. */
        char replyPacket[65536], modifytime[32], fileContent[65536], line[1024];
        FILE *fp = fopen("./index.html", "r");
        memset(fileContent, 0, sizeof(fileContent));
        while (fgets(line, sizeof(line), fp))
            strcat(fileContent, line);
        strcpy(replyPacket, replyHeader);
        char lenString[30];
        sprintf(lenString, "Content-Length: %d\r\n", (int)strlen(fileContent));
        strcat(replyPacket, lenString);
        struct stat attrib;
        stat("./index.html", &attrib);
        strftime(modifytime, 32, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&(attrib.st_mtime)));
        strcat(replyPacket, "Last-Modified: ");
        strcat(replyPacket, modifytime);
        strcat(replyPacket, "\r\n\r\n");
        strcat(replyPacket, fileContent);
        write(newsockfd, replyPacket, strlen(replyPacket));
        close(newsockfd);
    }
    close(sockfd);
}
