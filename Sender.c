#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>

#define ASSERT(_con, _msg)    if (!(_con)) {\
                                    perror((char *)(_msg));\
                                    exit(1);\
                                }

void send_buffer(int socket_fd, char* buffer, uint32_t buffer_length) {
    int nof_bytes = buffer_length;
    int sent_bytes;

    do {
        ASSERT((sent_bytes = write(socket_fd, buffer, nof_bytes)) >= 0, "write failed");
        nof_bytes -= sent_bytes;
        buffer += sent_bytes;

    } while(nof_bytes > 0);
}

int get_file_size(FILE *file_fd) {
    int file_length;
    fseek(file_fd, 0L, SEEK_END);
    file_length = ftell(file_fd);
    rewind(file_fd);
    return file_length;
}

void read_file(FILE *file_fd, int file_length, char *buffer) {
    char ch;
    int i = 0;
    while ((ch = fgetc(file_fd)) != EOF) {
        buffer[i] = ch;
        i++;
    }
}

void enc_file(char *buffer, int buffer_length, char *enc_buffer, int enc_buffer_length) {
    strcpy(enc_buffer, buffer);
}

int main(int argc, char* argv[]) {
    char *server_ip, *file_path;
    int server_port;
    int socket_fd;
    FILE *file_fd;
    char *buffer, *enc_buffer;
    uint32_t file_length, enc_buffer_length;
    struct sockaddr_in serv_addr;

    ASSERT(argc == 3, "argc value is to big / small");

    server_ip   =   argv[1];
    server_port =   atoi(argv[2]);

    /* Create TCP Socket */
    ASSERT((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0, "socket failed");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &(serv_addr.sin_addr));
    

    while (1) {
        ASSERT(connect(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) >= 0, "connect failed");

        printf("enter file name:\n");
        scanf("%s", file_path);

        if(strcmp(file_path, "quit") == 0) {
            close(socket_fd);
            break;
        }
        
        ASSERT((file_fd = fopen(file_path, "r")) >= 0, "open failed");

        /* Get and print file length */
        file_length = get_file_size(file_fd);
        printf("file length: %u bytes\n", file_length);

        /* Read file and send */
        ASSERT((buffer = (char *) calloc(file_length, sizeof(char))) != NULL, "calloc failed");
        read_file(file_fd, file_length, buffer);

        enc_buffer_length = file_length;
        ASSERT((enc_buffer = (char *) calloc(enc_buffer_length, sizeof(char))) != NULL, "calloc failed");

        enc_file(buffer, file_length, enc_buffer, enc_buffer_length);
        send_buffer(socket_fd, enc_buffer, enc_buffer_length);

        printf("sent: %u bytes\n", file_length);

        fclose(file_fd);
        close(socket_fd);
    }

    exit(0);
}

