#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define PORT     8080
#define BUF_SIZE 128

int main() {
    int sd;
    char buff[BUF_SIZE];
    struct sockaddr_in server_addr;

    // 소켓 생성
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        printf("can't create socket\n");
        return -1;
    }

    // 소켓 주소 구조체 초기화
    bzero((char *)&server_addr, sizeof(server_addr));

    // 서버 소켓 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    int n;
    socklen_t len;

    while(1) {
        printf("Input : ");
        if (fgets(buff, BUF_SIZE, stdin)) {
            buff[strlen(buff)-1] = '\0';
        } else {
            printf("fgets error\n");
            break;
        }

        // UDP server 를 종료한 후, sendto()를 실행하면 프로그램이 block 되는데, 어떻게 예외처리를 해야 할 까?
        // sendto()가 block 되는 이유는 OS 마다 다르다고 한다.
        if (sendto(sd, buff, strlen(buff), 0, (const struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
            printf("UDP server connection error : %s\n", strerror(errno));
            break;
        }

        if (strcmp(buff, "exit") == 0) {
            printf("UDP client exit\n");
            break;
        }

        n = recvfrom(sd, (char *)buff, BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &server_addr, &len);
        buff[n] = '\0';

        printf("Server(%s:%d) : %s\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port) ,buff);
    }

    close(sd);

    return 0;
}