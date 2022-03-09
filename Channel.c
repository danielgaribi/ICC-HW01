#include "Channel.h"

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