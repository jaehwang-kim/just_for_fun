#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ps_cmd.h"

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

void handle_request(int client_socket) {
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