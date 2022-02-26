#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>

#define ALLOC_BLOCK 1024
#define MAX_STR_LEN 1024

#define ASSERT(_con, _msg)    if (!(_con)) {\
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
        ASSERT((nof_bytes_rec = read(socket_fd, current_buffer_pointer, *buffer_length - total_nof_bytes_rec)) >= 0, "write failed");
        total_nof_bytes_rec += nof_bytes_rec;
        if (*buffer_length == total_nof_bytes_rec) {
            *buffer_length = ((*buffer_length / ALLOC_BLOCK) + 1) * ALLOC_BLOCK;
            ASSERT((buffer = (char *) realloc(buffer, *buffer_length * sizeof(char))) != NULL, "realloc failed");
        }
        current_buffer_pointer += total_nof_bytes_rec;

    } while(nof_bytes_rec > 0);

    *buffer_length = total_nof_bytes_rec;
    ASSERT((buffer = (char *) realloc(buffer, *buffer_length * sizeof(char))) != NULL, "realloc failed");

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
    ASSERT(fwrite(buffer, sizeof(char) ,file_length , file_fd) == file_length, "fwrite failed");
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

        /* Get decrypted buffer */
        enc_buffer = get_buffer(socket_fd, &enc_buffer_length);
        printf("received: %u bytes", enc_buffer_length);

        buffer_length = enc_buffer_length;
        ASSERT((buffer = (char *) calloc(buffer_length, sizeof(char))) != NULL, "calloc failed");

        nof_corrected_bytes = dec_file(buffer, buffer_length, enc_buffer, enc_buffer_length);
        free(enc_buffer);

        
        ASSERT((file_fd = fopen(file_path, "w")) >= 0, "open failed");
        write_file(file_fd, buffer_length, buffer);
        free(buffer);
        printf("wrote: %u bytes", buffer_length);
        printf("corrected %u errors", nof_corrected_bytes);
        
        fclose(file_fd);
        close(socket_fd);
    }

    exit(0);
}

