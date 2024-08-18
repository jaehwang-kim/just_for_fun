#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "handle_request.h"

#define PORT 8080

int create_socket() {
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

void bind_socket(int server_fd, struct sockaddr_in *address) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
}

void start_listening(int server_fd) {
    if (listen(server_fd, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief HTTP 서버의 메인 함수입니다.
 *
 * 이 함수는 HTTP 서버를 초기화하고 주요 이벤트 루프를 시작하는 역할을 담당합니다.
 * 네트워크 소켓 및 파일 디스크립터와 같은 필요한 리소스를 설정하고,
 * 들어오는 클라이언트 요청을 처리합니다. 주요 이벤트 루프는 계속해서
 * 들어오는 연결을 수신하고 요청을 처리하며 적절한 응답을 보냅니다.
 *
 * @return 서버가 성공적으로 실행되면 0을 반환하고, 그렇지 않으면 0이 아닌 값을 반환합니다.
 */
int httpd_main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 소켓 생성
    server_fd = create_socket();

    // 소켓 바인딩
    bind_socket(server_fd, &address);

    // 리스닝 시작
    start_listening(server_fd);

    while (1) {
        printf("Waiting for connections ...\n");

        // 연결 수락
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        // 클라이언트 처리
        handle_request(new_socket);
    }

    return 0;
}