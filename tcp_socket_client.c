#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 128

int main(int argc, char *argv[]) {
    /*
     * 1. local variable 선언
     *
     * s           : socket descriptor
     * server_addr : server 의 주소를 정의하는 구조체
     * ip_addr     : server 의 ip address
     * buf         : input, receiver buffer
     */
    int sd;
    struct sockaddr_in server_addr;
    char *ip_addr = "127.0.0.1";
    char buf[BUF_SIZE+1];

    /*
     * 2. socket 생성
     *
     * PF_INET     : 인터넷
     * SOCK_STREAM : TCP 타입의 socket
     */
    if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("can't create socket\n");
        return -1;
    }

    /*
     * 3. server_addr 초기화
     *
     * #include <string.h>
     * void bzero(void *s, size_t n);
     * byte string s의 첫바이트부터 n 바이트까지 0으로 채운다.
     */
    bzero((char *) &server_addr, sizeof(server_addr));

    /*
     * 4. server 주소 체계 설정
     *
     */
    server_addr.sin_family = AF_INET;

    /*
     * 5. server ip address 설정
     *
     * #include <arpa/inet.h>
     * in_addr_t inet_addr(const char *cp);
     * "a.b.c.d"형태의 IPv4 주소 체계를 network byte order의 binary 주소로 변환한다.
     * 주로, socket address family가 AF_INET 타입인 경우에 사용한다.
     */
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);

    /*
     * 6. server port 번호 설정
     * #include <arpa/inet.h>
     * uint32_t htonl (uint32_t hostlong);
     * long 타입 데이터를 host 시스템의 byte order 에서 네트워크의 byte order 로 변경해주는 함수.
     */
    server_addr.sin_port = htons(8080);

    /*
     * 7. server에 존재하는 socket과 연결 시도, 성공시 0 리턴
     * 실패시 -1을 리턴하며, 전역변수 errno에 에러코드가 들어있게 된다.
     */
    if (connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("can't connect : %d\n", errno);
        return -1;
    }

    /*
     * 8. connect 를 통해 성공적으로 세션이 맺어지면, 이제 Server 와 소켓 통신이 가능하다.
     */
    while(1) {
        // 사용자 입력
        printf("Input : ");
        if (fgets(buf, BUF_SIZE, stdin)) {
            buf[strlen(buf)-1] = '\0';
        } else {
            printf("fgets error\n");
            return -1;
        }

        // 입력값 서버로 전송
        if (send(sd, buf, strlen(buf), 0) < 0) {
            printf("send error : %d\n", errno);
            return -1;
        }

        if(strcmp(buf, "exit") == 0) {
            printf("tcp client exit!\n");
            break;
        }

        // 서버로부터 데이터 수신
        printf("Received Message : ");
        if (recv(sd, buf, sizeof(buf)-1, 0) < 0) {
            printf("recv error : %d\n", errno);
        }
        buf[BUF_SIZE] = '\0';

        printf("%s\n", buf);
    }

    close(sd);
}
