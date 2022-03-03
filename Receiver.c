/* gcc .\Receiver.c -lwsock32 -o .\Receiver.exe */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

#define ALLOC_BLOCK 1024
#define MAX_STR_LEN 1024

#define ASSERT(_con, _msg)    if (!(_con)) {\
                                    perror("Error!\n");\
                                    printf("%d\n", WSAGetLastError());\
                                    perror((char *)(_msg));\
                                    exit(1);\
                                }

char* get_buffer(int socket_fd, uint32_t *buffer_length) {
    int total_nof_bytes_rec = 0;
    int nof_bytes_rec;
    char* buffer, *current_buffer_pointer;
    ASSERT((buffer = (char *) calloc(ALLOC_BLOCK, sizeof(char))) != NULL, "calloc failed");
    *buffer_length = ALLOC_BLOCK;
    current_buffer_pointer = buffer;

    do {
        ASSERT((nof_bytes_rec = recv(socket_fd, current_buffer_pointer, *buffer_length - total_nof_bytes_rec, 0)) >= 0, "recv failed");
        printf("c: %d\n", current_buffer_pointer[0]);
        total_nof_bytes_rec += nof_bytes_rec;
        if (*buffer_length == total_nof_bytes_rec) {
            *buffer_length = ((*buffer_length / ALLOC_BLOCK) + 1) * ALLOC_BLOCK;
            ASSERT((buffer = (char *) realloc(buffer, *buffer_length * sizeof(char))) != NULL, "realloc failed");
        }
        current_buffer_pointer += total_nof_bytes_rec;

    } while(nof_bytes_rec > 0);

    *buffer_length = total_nof_bytes_rec;
    ASSERT((buffer = (char *) realloc(buffer, *buffer_length * sizeof(char))) != NULL, "realloc failed");

    printf("c0: %c\n", buffer[0]);
    return buffer;
}

int get_file_size(FILE *file_fd) {
    int file_length;
    fseek(file_fd, 0L, SEEK_END);
    file_length = ftell(file_fd);
    rewind(file_fd);
    return file_length;
}

void write_file(FILE *file_fd, int file_length, char *buffer) {
    ASSERT(fwrite(buffer, 1, file_length ,file_fd) == file_length, "fwrite failed");
}

uint32_t dec_file(char *buffer, int buffer_length, char *enc_buffer, int enc_buffer_length) {
    uint32_t nof_corrected_bytes = 0;
    strcpy(buffer, enc_buffer);
    return nof_corrected_bytes;
}

int main(int argc, char* argv[]) {
    char *server_ip, file_path[MAX_STR_LEN];
    int server_port;
    int socket_fd;
    FILE *file_fd;
    char *buffer, *enc_buffer;
    uint32_t buffer_length, enc_buffer_length, nof_corrected_bytes;
    struct sockaddr_in serv_addr;
    WSADATA wsaData;

    ASSERT(WSAStartup(MAKEWORD(2,2), &wsaData) == NO_ERROR, "WSAStartup failed");

    ASSERT(argc == 3, "argc value is to big / small");

    server_ip   =   argv[1];
    server_port =   atoi(argv[2]);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    serv_addr.sin_addr.s_addr = inet_addr(server_ip);
    

    while (1) {
        /* Create TCP Socket */
        ASSERT((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET, "socket failed");
        ASSERT(connect(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) != SOCKET_ERROR, "connect failed");

        printf("enter file name:\n");
        scanf("%s", file_path);

        if(strcmp(file_path, "quit") == 0) {
            closesocket(socket_fd);
            break;
        }

        printf("1\n");

        /* Get decrypted buffer */
        enc_buffer = get_buffer(socket_fd, &enc_buffer_length);
        printf("buf: %s\n", enc_buffer);
        printf("received: %u bytes\n", enc_buffer_length);
        printf("2\n");

        buffer_length = enc_buffer_length;
        ASSERT((buffer = (char *) calloc(buffer_length, sizeof(char))) != NULL, "calloc failed");

        nof_corrected_bytes = dec_file(buffer, buffer_length, enc_buffer, enc_buffer_length);
        free(enc_buffer);

        ASSERT((file_fd = fopen(file_path, "w+")) >= 0, "open failed");
        write_file(file_fd, buffer_length, buffer);
        free(buffer);
        printf("wrote: %u bytes\n", buffer_length);
        printf("corrected %u errors\n", nof_corrected_bytes);
        
        fclose(file_fd);
        closesocket(socket_fd);
    }

    exit(0);
}

