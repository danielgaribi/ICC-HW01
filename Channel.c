#include "Channel.h"

void encode_haming( short *msg ) {
    short msg_enc[ FRAME_NOF_BITS ] = { 0 }; 
    short C1, C2, C3, C4, C5;
    int data_index = 0;

    /* Set data bits in hamming index, set parity bits 0 for now */
    for ( int i = 0; i < FRAME_NOF_BITS; i++ ) { 
        if ( ( i == C1_INDEX ) || ( i == C2_INDEX ) || ( i == C3_INDEX ) || ( i == C4_INDEX ) || ( i == C5_INDEX ) ) {
            msg_enc[ i ] = 0;
        } else {
            msg_enc[ i ] = msg[data_index];
            data_index++;
        }
    }

    /* Calculate parity bits */
    C1 = msg_enc[ 0 ] ^ msg_enc[ 2 ] ^ msg_enc[ 4 ] ^ msg_enc[ 6 ] ^ msg_enc[ 8 ] ^ msg_enc[ 10 ]
    ^ msg_enc[ 12 ] ^ msg_enc[ 14 ] ^ msg_enc[ 16 ] ^ msg_enc[ 18 ] ^ msg_enc[ 20 ]
    ^ msg_enc[ 22 ] ^ msg_enc[ 24 ] ^ msg_enc[ 26 ] ^ msg_enc[ 28 ] ^ msg_enc[ 30 ];
    msg_enc[ 0 ] = C1;

    /* Calculate C2 */
    C2 = msg_enc[ 1 ] ^ msg_enc[ 2 ] ^ msg_enc[ 5 ] ^ msg_enc[ 6 ] ^ msg_enc[ 9 ] ^ msg_enc[ 10 ]
    ^ msg_enc[ 13 ] ^ msg_enc[ 14 ] ^ msg_enc[ 17 ] ^ msg_enc[ 18 ] ^ msg_enc[ 21 ]
    ^ msg_enc[ 22 ] ^ msg_enc[ 25 ] ^ msg_enc[ 26 ] ^ msg_enc[ 29 ] ^ msg_enc[ 30 ];
    msg_enc[ 1 ] = C2;

    /* Calculate C3 */
    C3 = msg_enc[ 3 ] ^ msg_enc[ 4 ] ^ msg_enc[ 5 ] ^ msg_enc[ 6 ] ^ msg_enc[ 11 ] ^ msg_enc[ 12 ]
    ^ msg_enc[ 13 ] ^ msg_enc[ 14 ] ^ msg_enc[ 19 ] ^ msg_enc[ 20 ] ^ msg_enc[ 21 ]
    ^ msg_enc[ 22 ] ^ msg_enc[ 27 ] ^ msg_enc[ 28 ] ^ msg_enc[ 29 ] ^ msg_enc[ 30 ];
    msg_enc[ 3 ] = C3;

    /* Calculate C4 */
    C4 = msg_enc[ 7 ] ^ msg_enc[ 8 ] ^ msg_enc[ 9 ] ^ msg_enc[ 10 ] ^ msg_enc[ 11 ] ^ msg_enc[ 12 ]
    ^ msg_enc[ 13 ] ^ msg_enc[ 14 ] ^ msg_enc[ 23 ] ^ msg_enc[ 24 ] ^ msg_enc[ 25 ]
    ^ msg_enc[ 26 ] ^ msg_enc[ 27 ] ^ msg_enc[ 28 ] ^ msg_enc[ 29 ] ^ msg_enc[ 30 ];
    msg_enc[ 7 ] = C4;

    /* Calculate C5 */
    C5 = msg_enc[ 15 ] ^ msg_enc[ 16 ] ^ msg_enc[ 17 ] ^ msg_enc[ 18 ] ^ msg_enc[ 19 ] ^ msg_enc[ 20 ]
    ^ msg_enc[ 21 ] ^ msg_enc[ 22 ] ^ msg_enc[ 23 ] ^ msg_enc[ 24 ] ^ msg_enc[ 25 ]
    ^ msg_enc[ 26 ] ^ msg_enc[ 27 ] ^ msg_enc[ 28 ] ^ msg_enc[ 29 ] ^ msg_enc[ 30 ];
    msg_enc[ 15 ] = C5;

    /* Copy back into original array */
    for ( int i = 0; i < 31; i++ ) {
        msg[ i ] = msg_enc[ i ];
    }
}

void decode_haming( short *msg_enc, short *msg_dec ) {
    short C1, C2, C3, C4, C5;
    int data_index = 0;
    int err_index = 0;

    /* Calculate parity bits */
    /* Calculate C1 - bits: 0101 0101 0101 0101 0101 0101 0101 0101 */
    C1 = msg_enc[ 0 ] ^ msg_enc[ 2 ] ^ msg_enc[ 4 ] ^ msg_enc[ 6 ] ^ msg_enc[ 8 ] ^ msg_enc[ 10 ]
    ^ msg_enc[ 12 ] ^ msg_enc[ 14 ] ^ msg_enc[ 16 ] ^ msg_enc[ 18 ] ^ msg_enc[ 20 ]
    ^ msg_enc[ 22 ] ^ msg_enc[ 24 ] ^ msg_enc[ 26 ] ^ msg_enc[ 28 ] ^ msg_enc[ 30 ];

    /* Calculate C2 - bits: 0110 0110 0110 0110 0110 0110 0110 0110 */
    C2 = msg_enc[ 1 ] ^ msg_enc[ 2 ] ^ msg_enc[ 5 ] ^ msg_enc[ 6 ] ^ msg_enc[ 9 ] ^ msg_enc[ 10 ]
    ^ msg_enc[ 13 ] ^ msg_enc[ 14 ] ^ msg_enc[ 17 ] ^ msg_enc[ 18 ] ^ msg_enc[ 21 ]
    ^ msg_enc[ 22 ] ^ msg_enc[ 25 ] ^ msg_enc[ 26 ] ^ msg_enc[ 29 ] ^ msg_enc[ 30 ];

    /* Calculate C3 - bits: 0111 1000 0111 1000 0111 1000 0111 1000 */
    C3 = msg_enc[ 3 ] ^ msg_enc[ 4 ] ^ msg_enc[ 5 ] ^ msg_enc[ 6 ] ^ msg_enc[ 11 ] ^ msg_enc[ 12 ]
    ^ msg_enc[ 13 ] ^ msg_enc[ 14 ] ^ msg_enc[ 19 ] ^ msg_enc[ 20 ] ^ msg_enc[ 21 ]
    ^ msg_enc[ 22 ] ^ msg_enc[ 27 ] ^ msg_enc[ 28 ] ^ msg_enc[ 29 ] ^ msg_enc[ 30 ];

    /* Calculate C4 - bits: 0111 1111 1000 0000 0111 1111 1000 0000 */
    C4 = msg_enc[ 7 ] ^ msg_enc[ 8 ] ^ msg_enc[ 9 ] ^ msg_enc[ 10 ] ^ msg_enc[ 11 ] ^ msg_enc[ 12 ]
    ^ msg_enc[ 13 ] ^ msg_enc[ 14 ] ^ msg_enc[ 23 ] ^ msg_enc[ 24 ] ^ msg_enc[ 25 ]
    ^ msg_enc[ 26 ] ^ msg_enc[ 27 ] ^ msg_enc[ 28 ] ^ msg_enc[ 29 ] ^ msg_enc[ 30 ];

    /* Calculate C5 -bits: 0111 1111 1111 1111 1000 0000 0000 0000 */
    C5 = msg_enc[ 15 ] ^ msg_enc[ 16 ] ^ msg_enc[ 17 ] ^ msg_enc[ 18 ] ^ msg_enc[ 19 ] ^ msg_enc[ 20 ]
    ^ msg_enc[ 21 ] ^ msg_enc[ 22 ] ^ msg_enc[ 23 ] ^ msg_enc[ 24 ] ^ msg_enc[ 25 ]
    ^ msg_enc[ 26 ] ^ msg_enc[ 27 ] ^ msg_enc[ 28 ] ^ msg_enc[ 29 ] ^ msg_enc[ 30 ];

    /* Fix msg if needed */
    err_index += C1_WEIGHT * C1 + C2_WEIGHT * C2 + C3_WEIGHT * C3 + C4_WEIGHT * C4 + C5_WEIGHT * C5;
    if ( err_index != 0 ) {
        msg_enc[ err_index ] = ( msg_enc[ err_index ] == 1 ) ? 0 : 1; // Flip bit 
    } // Else - error not detected 

    /* Extract data from encoded msg */
    for ( int i = 0; i < FRAME_NOF_BITS; i++ ) { 
        if ( ( i == C1_INDEX ) || ( i == C2_INDEX ) || ( i == C3_INDEX ) || ( i == C4_INDEX ) || ( i == C5_INDEX ) ) {
            continue;
        } else {
            msg_dec[ data_index ] = msg_enc[ i ];
            data_index++;
        }
    }
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

void send_buffer(int socket_fd, char* buffer, uint32_t buffer_length) {
    int nof_bytes = buffer_length;
    int sent_bytes;

    do {
        ASSERT((sent_bytes = send(socket_fd, buffer, nof_bytes, 0)) >= 0, "send failed");
        nof_bytes -= sent_bytes;
        buffer += sent_bytes;

    } while(nof_bytes > 0);
}

int addNoise( char *msg, uint32_t msg_size, noiseType noise_type, int prob, int seed, int n ) {
    int NumChangedBits = 0; 
    if ( noise_type == noNoise ) { // debug
        return NumChangedBits;
    }

    if ( noise_type == randomy ) {
        srand( seed );
        for ( int i = 0; i < msg_size; i++ ) {
            char byte = msg[ i ];
            uint8_t mask = 1; 
            for ( int bit = 0; bit <= 7; bit++ ) {
                if ( ( rand() % MAX_RAND ) < prob ) {
                    byte = byte ^ mask;
                    NumChangedBits++;
                } 
                mask = mask * 2;
            }
            msg[ i ] = byte;
        }
    } else { // noise_type == deterministic
        int cnt = 1;
        for ( int i = 0; i < msg_size; i++ ) {
            char byte = msg[ i ];
            uint8_t mask = 1; 
            for ( int bit = 0; bit <= 7; bit++ ) {
                if ( cnt == n ) {
                    byte = byte ^ mask;
                    NumChangedBits++;
                    cnt = 0;
                } 
                cnt++;
                mask = mask * 2;
            }
            msg[ i ] = byte;
        }
    }
    return NumChangedBits;
}

int setup_listen_socket(int server_port, char *type) {
    struct sockaddr_in serv_addr;
    int listen_fd;

    ASSERT((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET, "socket failed");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 
    ASSERT(bind(listen_fd, (SOCKADDR *) &serv_addr, sizeof(serv_addr)) == 0, "bind failed");
    ASSERT(listen(listen_fd, LISTEN_QUEUE_SIZE)== 0, "listen failed");


    printf("%s socket: IP address = %s port = %d\n", type, inet_ntoa(serv_addr.sin_addr), server_port); 

    return listen_fd;
}

int main(int argc, char* argv[]) {
    char *buffer;
    char user_input[MAX_STR_LEN];
    int buffer_size;
    struct sockaddr_in peer_addr;
    int addrsize = sizeof(struct sockaddr_in);
    int sender_listen_fd, receiver_listen_fd;
    int sender_fd, receiver_fd;
    WSADATA wsaData;

    ASSERT(WSAStartup(MAKEWORD(2,2), &wsaData) == NO_ERROR, "WSAStartup failed");

    int prob            = -1;
    int seed            = -1; 
    int n               = -1;
    int NumChangedBits  = -1;
    noiseType noise_type;

    ASSERT(argc == 3 || argc == 4, "argc value is to big / small");

    char *noise_type_str = argv[ 1 ]; // debug: Assuming that type of noise will be set as first argument 
    if ( strcmp( noise_type_str, "-r") == 0 ) {
        ASSERT(argc == 4, "Invalid number of args");
        prob = atoi( argv[ 2 ] );
        seed = atoi( argv[ 3 ] );
        noise_type = randomy;
    } else if ( strcmp( noise_type_str, "-d") == 0 ) {
        ASSERT(argc == 3, "Invalid number of args");
        n = atoi( argv[ 2 ] );
        noise_type = deterministic;
    } else if ( strcmp( noise_type_str, "-n") == 0 ) { // debug: For debug 
        noise_type = noNoise; 
    } else {
        ASSERT(0, "Invalid noise type, use -r for random and -d of args");
    }

    sender_listen_fd = setup_listen_socket(SENDER_PORT, "sender");
    receiver_listen_fd = setup_listen_socket(RECEIVER_PORT, "receiver");

    while (1) {
        ASSERT((sender_fd = accept(sender_listen_fd, (SOCKADDR *) &peer_addr, &addrsize)) != INVALID_SOCKET, "accept sender failed");
        ASSERT((receiver_fd = accept(receiver_listen_fd, (SOCKADDR *) &peer_addr, &addrsize)) != INVALID_SOCKET, "accept receiver failed");

        buffer = get_buffer(sender_fd, &buffer_size);
        NumChangedBits = addNoise(buffer, buffer_size, noise_type, prob, seed, n); 
        printf("retransmitted %d bytes, flipped %d bits\n", buffer_size, NumChangedBits);
        send_buffer(receiver_fd, buffer, buffer_size);

        free(buffer);

        closesocket(sender_fd);
        closesocket(receiver_fd);

        printf("continue? (yes/no)");
        scanf("%s", user_input);
        if ( strcmp("yes", user_input) == 0 ) {
            continue;
        } else if (strcmp("no", user_input) == 0) {
            break;
        } else {
            ASSERT(0, "Invalid answer - yes to continue, no to quit");
        }

    }
    closesocket(sender_listen_fd);
    closesocket(receiver_listen_fd);
}