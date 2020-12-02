#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUF_SIZE 128

int main(int argc, char *argv[]) {
    /*
     * 1. local variable 선언
     *
     * server_sd       : server socket 번호
     * client_sd       : client socket 번호
     * len_out         : client 로부터 받은 데이터의 길이
     * port            : socket 에 바인딩 될 서버의 port 번호
     * buf             : client 와의 통신에서 사용할 버퍼
     * server_addr     : server socket 주소정보를 담고 있는 구조체
     * client_addr     : client socket 주소정보를 담고 있는 구조체
     * client_addr_len : client_addr 변수의 길이
     */
    int server_sd, client_sd; // socket 번호
    int len_out;
    int port;
    char buf[BUF_SIZE+1];

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    port = 8080;

    /*
     * 2. socket 생성
     */
    if ((server_sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("can't open stream socket");
        return 0;
    }

    /*
     * 3. server_addr 초기화
     */
    bzero((char *)&server_addr, sizeof(server_addr));

    /*
     * 4. server 주소 체계 설정
     * 5. server ip address 설정
     * 6. server port 번호 설정
     */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);


    /*
     * 7. socket 과 socket 주소를 바인딩
     */
    if (bind(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("can't bind local address\n");
        return 0;
    }

    /*
     * 8. 소켓을 수동 대기모드로 세팅
     *
     * listen()의 두번째 파라미터는, 대기가능한 총 client connection 수이다.
     * 설정된 값보다 많은 수의 연결이 왔을 때, 설정값 이후의 connection은 거절하여 클라이언트가 바로 알 수 있게 한다.
     */
    listen(server_sd, 5);

    /*
     * 9. accept()에서 block되며, client의 연결을 기다린다.
     */
    while(1) {
        printf("Waiting connection request...\n");

        client_addr_len = sizeof(client_addr);
        client_sd = accept(server_sd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sd < 0) {
            printf("accept failed\n");
            break;
        }

        /*
         * 9. client와 연결되고, 세션이 확립 establish 된 상태
         *    recv, send를 통해 client와 소켓 통신
         */
        printf("client connected\n");
        while(1) {
            bzero(buf, sizeof(buf));
            len_out = recv(client_sd, buf, sizeof(buf), 0);
            if (len_out < 0) {
                printf("read error : %d\n", errno);
                break;
            } else if (len_out == 0) {
                printf("client's close socket\n");
                break;
            }

            printf("client : %s\n", buf);
            write(client_sd, buf, len_out);
        }

        close(client_sd);
    }

    close(server_sd);

    return 0;
}