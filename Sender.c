/* gcc .\Sender.c -lwsock32 -o .\Sender.exe */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

#define MAX_STR_LEN 1024

#define FRAME_NOF_DATA_BITS     26
#define FRAME_NOF_BITS          31
#define NOF_FRAMS_IN_PACKET     8
#define NOF_BITS_IN_BYTE        8
#define DATA_BUFFER_LENFTH      CONVERT_BIT_TO_BYTES(FRAME_NOF_DATA_BITS * NOF_FRAMS_IN_PACKET)
#define ENC_BUFFER_LENFTH       CONVERT_BIT_TO_BYTES(FRAME_NOF_BITS * NOF_FRAMS_IN_PACKET)

#define CONVERT_BIT_TO_BYTES(_nof_bits)    ((_nof_bits) / NOF_BITS_IN_BYTE)

#define ASSERT(_con, _msg)    if (!(_con)) {\
                                    perror("Error!\n");\
                                    printf("%d\n", WSAGetLastError());\
                                    perror((char *)(_msg));\
                                    exit(1);\
                                }

void send_buffer(int socket_fd, char* buffer, uint32_t buffer_length) {
    int nof_bytes = buffer_length;
    int sent_bytes;

    for(int i = 0; i < buffer_length; i++){
        printf("%d: '%u'\n", i, buffer[i]);
    }

    do {
        ASSERT((sent_bytes = send(socket_fd, buffer, nof_bytes, 0)) >= 0, "send failed");
        nof_bytes -= sent_bytes;
        buffer += sent_bytes;

    } while(nof_bytes > 0);
}

int get_file_size(FILE *file_fd) {
    int file_length;
    fseek(file_fd, 0L, SEEK_END);
    file_length = ftell(file_fd);
    fseek(file_fd, 0, SEEK_SET);
    // rewind(file_fd);
    return file_length;
}

int read_frame_from_file(FILE *file_fd, int file_length, char *buffer) {
    int nof_read_bytes = 0;
    int nof_remain_bytes = CONVERT_BIT_TO_BYTES(FRAME_NOF_DATA_BITS * NOF_FRAMS_IN_PACKET);
    int total_nof_read_bytes = 0;
    char *working_buffer = buffer;


    // while ((nof_read_bytes = read(file_fd, buffer, 1000)) != nof_remain_bytes) {
    while ((nof_read_bytes = fread(working_buffer, sizeof(char), nof_remain_bytes, file_fd)) != nof_remain_bytes) {
        nof_remain_bytes -= nof_read_bytes;
        working_buffer += nof_read_bytes;
        total_nof_read_bytes += nof_read_bytes;
    }
    total_nof_read_bytes += nof_read_bytes;
    return total_nof_read_bytes;
    // printf("nof: %d\n", nof_remain_bytes);
    // for(int i = 0; i < file_length; i++){
    //     printf("%d: '%u'\n", i, buffer[i]);
    // }

    // char ch;
    // int i = 0;

    // while ((ch = fgetc(file_fd)) != EOF) {
    //     buffer[i] = ch;
    //     i++;
    // }
    // printf("i=%d\n", i);
    // ASSERT(i == file_length, "Read Failed");
}

void convert_bit_array_to_buffer(char *enc_buffer, int enc_buffer_length, short *bits_array, int bits_array_length) {
    int i,j, index = 0;
    char c;

    for(i = 0; i < enc_buffer_length; i++) {
        c = 0;
        for(j = 0; j < sizeof(short); j++) {
            c |= bits_array[index];
            c <= 1;
            index++;
        }
        enc_buffer[i] = c;
    }
}

void enc_file(char *buffer, int buffer_length, char *enc_buffer, int enc_buffer_length) {
    short bits_array[FRAME_NOF_BITS * NOF_FRAMS_IN_PACKET] = { 0 };
    short *sub_bits_array = bits_array;
    short mask;
    int i, j, index = 0;
    for(i = 0; i < buffer_length; i++) {
        mask = 1 << (sizeof(short) - 1);
        for(j = 0; j < sizeof(short); j++) {
            sub_bits_array[index] = buffer[i] & mask;
            index++;
            if (index == FRAME_NOF_DATA_BITS) {
                /* hamming!!! */
                sub_bits_array += FRAME_NOF_BITS;
                index = 0;
            }
            mask >>= 1;
        }
    }
    convert_bit_array_to_buffer(enc_buffer, enc_buffer_length, bits_array, FRAME_NOF_BITS * NOF_FRAMS_IN_PACKET);
}

int main(int argc, char* argv[]) {
    char *server_ip, file_path[MAX_STR_LEN];
    int server_port;
    int socket_fd;
    FILE *file_fd;
    uint32_t file_length, data_buffer_length, enc_buffer_length, read_bytes, sent_bytes;
    struct sockaddr_in serv_addr;
    WSADATA wsaData;

    char data_buffer[DATA_BUFFER_LENFTH] = { 0 };
    char enc_buffer[ENC_BUFFER_LENFTH] = { 0 };

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
        ASSERT((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) != 0, "socket failed");
        ASSERT(connect(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) >= 0, "connect failed");

        printf("enter file name:\n");
        scanf("%s", file_path);

        printf("1\n");
        if(strcmp(file_path, "quit") == 0) {
            closesocket(socket_fd);
            break;
        }
        
        ASSERT((file_fd = fopen(file_path, "rb")) >= 0, "open failed");

        /* Get and print file length */
        file_length = get_file_size(file_fd);
        // file_length = 80;
        printf("file length: %u bytes\n", file_length);

        read_bytes = 0;
        sent_bytes = 0;
        /* Read file and send */
        while(file_length != read_bytes) {
            read_bytes += read_frame_from_file(file_fd, file_length, data_buffer);
            enc_file(data_buffer, data_buffer_length, enc_buffer, enc_buffer_length);
            send_buffer(socket_fd, enc_buffer, enc_buffer_length);
            sent_bytes += enc_buffer_length;
        }

        // /* Read file and send */
        // read_file(file_fd, file_length, buffer);

        // enc_buffer_length = file_length;
        // ASSERT((enc_buffer = (char *) calloc(enc_buffer_length, sizeof(char))) != NULL, "calloc failed");

        // enc_file(buffer, file_length, enc_buffer, enc_buffer_length);
        // for(int i = 0; i < enc_buffer_length; i++){
        //     printf("%d: '%u'\n", i, enc_buffer[i]);
        // }
        // free(buffer);
        // send_buffer(socket_fd, enc_buffer, enc_buffer_length);
        // free(enc_buffer);

        printf("sent: %u bytes\n", sent_bytes);

        fclose(file_fd);
        closesocket(socket_fd);
    }

    exit(0);
}
