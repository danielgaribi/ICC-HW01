/* gcc .\Receiver.c -lwsock32 -o .\Receiver.exe */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

#define ALLOC_BLOCK             1024
#define MAX_STR_LEN             1024

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

int get_buffer(int socket_fd, char* enc_buffer, uint32_t buffer_length) {
    int total_nof_bytes_rec = 0;
    int nof_bytes_rec;
    char* current_buffer_pointer = enc_buffer;

    do {
        ASSERT((nof_bytes_rec = recv(socket_fd, current_buffer_pointer, buffer_length - total_nof_bytes_rec, 0)) >= 0, "recv failed");
        total_nof_bytes_rec += nof_bytes_rec;
        if (buffer_length == total_nof_bytes_rec) {
            break;
        }
        current_buffer_pointer += total_nof_bytes_rec;

    } while(nof_bytes_rec > 0);

    return total_nof_bytes_rec;

    // *buffer_length = total_nof_bytes_rec;
    // ASSERT((buffer = (char *) realloc(buffer, *buffer_length * sizeof(char))) != NULL, "realloc failed");

    // for(int i = 0; i < *buffer_length; i++){
    //     printf("%d: '%u'\n", i, buffer[i]);
    // }
}

uint32_t write_file(FILE *file_fd, uint32_t buffer_length, char *buffer) {
    uint32_t nof_write_bytes = 0;
    ASSERT((nof_write_bytes = fwrite(buffer, 1, buffer_length ,file_fd)) == buffer_length, "fwrite failed");
    return nof_write_bytes;
}

void convert_bit_array_to_buffer(char *buffer, int buffer_length, short *bits_array, int bits_array_length) {
    int i,j, index = 0;
    char c;

    for(i = 0; i < buffer_length; i++) {
        c = 0;
        for(j = 0; j < sizeof(short); j++) {
            c |= bits_array[index];
            c <= 1;
            index++;
        }
        buffer[i] = c;
    }
}

int dec_file(char *buffer, int buffer_length, char *enc_buffer, int enc_buffer_length) {
    short data_bits_array[FRAME_NOF_DATA_BITS * NOF_FRAMS_IN_PACKET] = { 0 };
    short enc_bits_array[FRAME_NOF_BITS] = { 0 };
    short *sub_data_bits_array = data_bits_array;
    short mask;
    int i, j, index = 0;
    int nof_corrected_bytes = 0;
    for(i = 0; i < enc_buffer_length; i++) {
        mask = 1 << (sizeof(short) - 1);
        for(j = 0; j < sizeof(short); j++) {
            enc_bits_array[index] = buffer[i] & mask;
            index++;
            if (index == FRAME_NOF_BITS) {
                /* hamming!!! */
                nof_corrected_bytes += 0;
                sub_data_bits_array += FRAME_NOF_DATA_BITS;
                index = 0;
            }
            mask >>= 1;
        }
    }
    convert_bit_array_to_buffer(buffer, buffer_length, data_bits_array, FRAME_NOF_DATA_BITS * NOF_FRAMS_IN_PACKET);
    return nof_corrected_bytes;
}

// uint32_t dec_file(char *buffer, int buffer_length, char *enc_buffer, int enc_buffer_length) {
//     uint32_t nof_corrected_bytes = 0;
//     for(int i = 0; i < enc_buffer_length; i++){
//         buffer[i] = enc_buffer[i];
//     }
//     return nof_corrected_bytes;
// }

int main(int argc, char* argv[]) {
    char *server_ip, file_path[MAX_STR_LEN];
    int server_port;
    int socket_fd;
    FILE *file_fd;
    uint32_t data_buffer_length, enc_buffer_length, nof_corrected_bytes, nof_write_bytes, recieved_bytes;
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
        ASSERT((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET, "socket failed");
        ASSERT(connect(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) != SOCKET_ERROR, "connect failed");

        printf("enter file name:\n");
        scanf("%s", file_path);

        if(strcmp(file_path, "quit") == 0) {
            closesocket(socket_fd);
            break;
        }

        ASSERT((file_fd = fopen(file_path, "wb")) >= 0, "open failed");

        recieved_bytes = 0;
        nof_corrected_bytes += 0;
        nof_write_bytes += 0;
        while (1) {
            recieved_bytes += get_buffer(socket_fd, enc_buffer, enc_buffer_length);
            if (recieved_bytes == 0) {
                break;
            }
            nof_corrected_bytes += dec_file(data_buffer, data_buffer_length, enc_buffer, enc_buffer_length);
            nof_write_bytes += write_file(file_fd, data_buffer_length, data_buffer);
        } 
        

        /* Get decrypted buffer */
        // enc_buffer = get_buffer(socket_fd, &enc_buffer_length);
        // printf("buf: %s\n", enc_buffer);
        // printf("received: %u bytes\n", recieved_bytes);

        // buffer_length = enc_buffer_length;
        // ASSERT((buffer = (char *) calloc(buffer_length, sizeof(char))) != NULL, "calloc failed");

        // nof_corrected_bytes = dec_file(buffer, buffer_length, enc_buffer, enc_buffer_length);
        // free(enc_buffer);

        // ASSERT((file_fd = fopen(file_path, "wb")) >= 0, "open failed");
        // write_file(file_fd, buffer_length, buffer);
        // free(buffer);
        printf("wrote: %u bytes\n", nof_write_bytes);
        printf("corrected %u errors\n", nof_corrected_bytes);
        
        fclose(file_fd);
        closesocket(socket_fd);
    }

    exit(0);
}

