#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ps_cmd.h"

#define PORT 8080

void handle_client(int client_socket) {
    char buffer[1024];
    int len;
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    char client_ip[INET_ADDRSTRLEN];

    // Get the client address information
    if (getpeername(client_socket, (struct sockaddr *)&client_address, &client_address_len) == -1) {
        perror("getpeername failed");
        close(client_socket);
        return;
    }

    // Convert the IP address to a string
    memset(client_ip, 0, sizeof(client_ip));

    inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);

    printf("Client IP address: %s\n\n", client_ip);

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

int httpd_main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    // 소켓 바인딩
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }

    // 리스닝
    if (listen(server_fd, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    while(1) {
        printf("Waiting for connections ...\n");

        // 연결 수락
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        // 클라이언트 처리
        handle_client(new_socket);
    }

    return 0;
}