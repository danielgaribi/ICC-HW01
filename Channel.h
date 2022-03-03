#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
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
    randomy  = 0,
    deterministic  = 1,
    noNoise = 2, // debug: For debug 
} noiseType;

#define ZERO_MASK           0x00000000      // 0000 0000 0000 0000 0000 0000 0000 0000
#define C1_MASK             0x00000001      // 0000 0000 0000 0000 0000 0000 0000 0001
#define C2_MASK             0x00000002      // 0000 0000 0000 0000 0000 0000 0000 0010
#define C3_MASK             0x00000010      // 0000 0000 0000 0000 0000 0000 0001 0000
#define C4_MASK             0x00000100      // 0000 0000 0000 0000 0000 0001 0000 0000
#define C5_MASK             0x00010000      // 0000 0000 0000 0001 0000 0000 0000 0000 

/* Masks for encoding */
#define ENC_DATA_0_MASK         0x00000001      // 0000 0000 0000 0000 0000 0000 0000 0001
#define ENC_DATA_1_3_MASK       0x0000000E      // 0000 0000 0000 0000 0000 0000 0000 1110
#define ENC_DATA_4_10_MASK      0x000007F0      // 0000 0000 0000 0000 0000 0111 1111 0000
#define ENC_DATA_11_25_MASK     0x03FFF800      // 0000 0011 1111 1111 1111 1000 0000 0000

#define ENC_SHIFT_DATA_0            2
#define ENC_SHIFT_DATA_1_3          4
#define ENC_SHIFT_DATA_4_10         8
#define ENC_SHIFT_DATA_11_25        16

#define SET_DATA_0( msg, msg_enc )        ( msg_enc ) | ( ( ( msg ) & ENC_DATA_0_MASK ) << ENC_SHIFT_DATA_0 )
#define SET_DATA_1_3( msg, msg_enc )      ( msg_enc ) | ( ( ( msg ) & ENC_DATA_1_3_MASK ) << ENC_SHIFT_DATA_1_3 )
#define SET_DATA_4_10( msg, msg_enc )     ( msg_enc ) | ( ( ( msg ) & ENC_DATA_4_10_MASK ) << ENC_SHIFT_DATA_4_10 )
#define SET_DATA_11_25( msg, msg_enc )    ( msg_enc ) | ( ( ( msg ) & ENC_DATA_11_25_MASK ) << ENC_SHIFT_DATA_11_25 )

/* Macros for calculating parity bits */ 
#define C1_PARITY_MASK 0x55555555   // 0101 0101 0101 0101 0101 0101 0101 0101
#define C2_PARITY_MASK 0x66666666   // 0110 0110 0110 0110 0110 0110 0110 0110
#define C3_PARITY_MASK 0x78787878   // 0111 1000 0111 1000 0111 1000 0111 1000
#define C4_PARITY_MASK 0x7F807F80   // 0111 1111 1000 0000 0111 1111 1000 0000
#define C5_PARITY_MASK 0x7FFF8000   // 0111 1111 1111 1111 1000 0000 0000 0000

/* Masks for decoding */
#define DEC_DATA_0_MASK         0x00000004  // 0000 0000 0000 0000 0000 0000 0000 0100
#define DEC_DATA_1_3_MASK       0x00000070  // 0000 0000 0000 0000 0000 0000 0111 0000
#define DEC_DATA_4_10_MASK      0x0000EF00  // 0000 0000 0000 0000 0111 1111 0000 0000
#define DEC_DATA_11_25_MASK     0xEFFF0000  // 0111 1111 1111 1111 0000 0000 0000 0000

#define DEC_SHIFT_DATA_0            2
#define DEC_SHIFT_DATA_1_3          3
#define DEC_SHIFT_DATA_4_10         4
#define DEC_SHIFT_DATA_11_25        5

#define GET_DATA_0( msg_dec, msg_enc )        ( msg_dec ) | ( ( ( msg_enc ) & DEC_DATA_0_MASK ) >> DEC_SHIFT_DATA_0 )
#define GET_DATA_1_3( msg_dec, msg_enc )      ( msg_dec ) | ( ( ( msg_enc ) & DEC_DATA_1_3_MASK ) >> DEC_SHIFT_DATA_1_3 )
#define GET_DATA_4_10( msg_dec, msg_enc )     ( msg_dec ) | ( ( ( msg_enc ) & DEC_DATA_4_10_MASK ) >> DEC_SHIFT_DATA_4_10 )
#define GET_DATA_11_25( msg_dec, msg_enc )    ( msg_dec ) | ( ( ( msg_enc ) & DEC_DATA_11_25_MASK ) >> DEC_SHIFT_DATA_11_25 )
