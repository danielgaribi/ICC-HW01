#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

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
#define MAX_RAND            1 << 16

#define ALLOC_BLOCK         1024
#define SENDER_PORT         6342
#define RECEIVER_PORT       6343
#define LISTEN_QUEUE_SIZE   10      /* TODO 10???????????????????????? */

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

char* get_buffer(int socket_fd, uint32_t* buffer_length);
void send_buffer(int socket_fd, char* buffer, uint32_t buffer_length);
int addNoise(char* msg, uint32_t msg_size, noiseType noise_type, int prob, int seed, int n);
int setup_listen_socket(int server_port, char* type);
int main(int argc, char* argv[]);