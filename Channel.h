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

#define FRAME_NOF_BITS 31

#define C1_INDEX    0
#define C2_INDEX    1
#define C3_INDEX    3
#define C4_INDEX    7
#define C5_INDEX    15

#define C1_WEIGHT    1
#define C2_WEIGHT    2
#define C3_WEIGHT    4
#define C4_WEIGHT    8
#define C5_WEIGHT    16