#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT     8080
#define BUF_SIZE 128

int main() {
    int server_sd;
    char buff[BUF_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;

    // 소켓 생성
    if ((server_sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        printf("can't open diagram socket\n");
        return 0;
    }

    // 소켓 주소 구조체 초기화
    bzero((char *)&server_addr, sizeof(server_addr));
    bzero((char *)&client_addr, sizeof(client_addr));

    // 서버 소켓 주소 설정
    server_addr.sin_family    = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // 서버 소켓에 주소 바인딩
    if (bind(server_sd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 ) {
        printf("can't bind socket\n");
        return 0;
    }

    int n;
    client_addr_len = sizeof(client_addr);  //len is value/resuslt

    while(1) {
        bzero(buff, sizeof(buff));
        n = recvfrom(server_sd, (char *)buff, BUF_SIZE, MSG_WAITALL, ( struct sockaddr *) &client_addr, &client_addr_len);
        buff[n] = '\0';

        if (strcmp(buff, "exit") == 0) {
            printf("UDP server stop\n");
            break;
        }

        printf("Client(%s:%d) : %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buff);
        sendto(server_sd, buff, strlen(buff), 0, (const struct sockaddr *) &client_addr, client_addr_len);
    }



    return 0;
}