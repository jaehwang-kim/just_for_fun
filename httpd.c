#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ps_cmd.h"

#define PORT 8080

int print_client_info(int client_socket, struct sockaddr_in *client_address) {
    char client_ip[INET_ADDRSTRLEN];
    socklen_t client_address_len = sizeof(*client_address);

    // 클라이언트 주소 정보 가져오기
    if (getpeername(client_socket, (struct sockaddr *)client_address, &client_address_len) == -1) {
        perror("getpeername failed");
   
       return -1;
    }

    // IP 주소를 문자열로 변환
    memset(client_ip, 0, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(client_address->sin_addr), client_ip, INET_ADDRSTRLEN);

    printf("Client IP address: %s\n\n", client_ip);

    return 0;
}

void handle_client(int client_socket) {
    char buffer[1024];
    int len;
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    int err;

    // 클라이언트 정보 출력
    err = print_client_info(client_socket, &client_address);    

    if (err) {
        close(client_socket);
        return;
    }

    memset(buffer, 0, sizeof(buffer));
    len = read(client_socket, buffer, 1024-1);
 
    printf("Received request: %s\n", buffer);

    char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nHello, World!";

    // client_socket을 FILE *로 변환`
    FILE *stream = fdopen(client_socket, "w+");
    if (stream == NULL) {
        perror("fdopen failed");
        close(client_socket);
        return;
    }

    // FILE *를 사용하여 응답을 클라이언트에게 보냄
    fprintf(stream, "%s", response);

    ps_cmd(stream);

    fflush(stream); // 스트림에 쓰여진 데이터를 강제로 출력

    // FILE * 스트림을 닫고, 이와 연결된 파일 디스크립터도 닫힘
    fclose(stream);
}


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
        handle_client(new_socket);
    }

    return 0;
}