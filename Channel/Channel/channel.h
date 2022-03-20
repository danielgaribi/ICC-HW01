#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>
#include <winsock2.h>

#define FALSE               0
#define TRUE                1

#define MAX_STR_LEN         1024 /* TODO 1024 ???????????????????????? */
#define MAX_RAND_VALUE      65536

#define ALLOC_BLOCK         1024
#define LISTEN_QUEUE_SIZE   10      /* TODO 10???????????????????????? */

#define FRAME_NOF_DATA_BITS     26
#define FRAME_NOF_BITS          31
#define NOF_FRAMS_IN_PACKET     8
#define NOF_BITS_IN_BYTE        8
#define DATA_BUFFER_LENGTH      CONVERT_BIT_TO_BYTES(FRAME_NOF_DATA_BITS * NOF_FRAMS_IN_PACKET)
#define ENC_BUFFER_LENGTH       CONVERT_BIT_TO_BYTES(FRAME_NOF_BITS * NOF_FRAMS_IN_PACKET)

#define CONVERT_BIT_TO_BYTES(_nof_bits)    ((_nof_bits) / NOF_BITS_IN_BYTE)
#define CONVERT_BYTES_TO_BITS(_nof_bytes)  ((_nof_bytes) * NOF_BITS_IN_BYTE)

#define ASSERT(_con, _msg)    if (!(_con)) {\
                                    perror("Error!\n");\
                                    printf("%d\n", WSAGetLastError());\
                                    perror((char *)(_msg));\
                                    exit(1);\
                                }

typedef enum noiseType {
    randomy = 0,
    deterministic = 1,
    noNoise = 2, // debug: For debug 
} noiseType;

uint32_t get_buffer(int socket_fd, char* buffer, uint32_t buffer_length);
uint32_t send_buffer(int socket_fd, char* buffer, uint32_t buffer_length);
int addNoise(char* msg, uint32_t msg_size, noiseType noise_type, int prob, int seed, int n);
int setup_listen_socket(int server_port, char* type);
int main(int argc, char* argv[]);