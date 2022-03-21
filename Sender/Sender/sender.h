#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

#define MAX_STR_LEN 1024

#define FRAME_NOF_DATA_BITS     26
#define FRAME_NOF_BITS          31
#define NOF_FRAMS_IN_PACKET     8
#define NOF_BITS_IN_BYTE        8
#define DATA_BUFFER_LENGTH      CONVERT_BIT_TO_BYTES(FRAME_NOF_DATA_BITS * NOF_FRAMS_IN_PACKET)
#define ENC_BUFFER_LENGTH       CONVERT_BIT_TO_BYTES(FRAME_NOF_BITS * NOF_FRAMS_IN_PACKET)

#define C1_INDEX                0
#define C2_INDEX                1
#define C3_INDEX                3
#define C4_INDEX                7
#define C5_INDEX                15

#define CONVERT_BIT_TO_BYTES(_nof_bits)    ((_nof_bits) / NOF_BITS_IN_BYTE)
#define CONVERT_BYTES_TO_BITS(_nof_bytes)  ((_nof_bytes) * NOF_BITS_IN_BYTE)

#define ASSERT(_con, _msg)    if (!(_con)) {\
                                    perror("Error!\n");\
                                    perror((char *)(_msg));\
                                    exit(1);\
                                }

void encode_haming(short* msg);
uint32_t send_buffer(int socket_fd, char* buffer, uint32_t buffer_length);
int get_file_size(FILE* file_fd);
int read_frame_from_file(FILE* file_fd, int file_length, char* buffer);
void convert_bit_array_to_buffer(char* enc_buffer, int* enc_buffer_length, short* bits_array, int bits_array_length);
void enc_file(char* buffer, uint32_t buffer_length, char* enc_buffer, uint32_t* enc_buffer_length);
int main(int argc, char* argv[]);