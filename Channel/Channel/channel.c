#include "channel.h"

uint32_t get_buffer(int socket_fd, char* buffer, uint32_t buffer_length) {
    int total_nof_bytes_rec = 0;
    int nof_bytes_rec;
    char* current_buffer_pointer = buffer;

    do {
        ASSERT((nof_bytes_rec = recv(socket_fd, current_buffer_pointer, buffer_length - total_nof_bytes_rec, 0)) >= 0, "recv failed");
        total_nof_bytes_rec += nof_bytes_rec;
        if (buffer_length == total_nof_bytes_rec) {
            break;
        }
        current_buffer_pointer += total_nof_bytes_rec;

    } while (nof_bytes_rec > 0);

    return total_nof_bytes_rec;
}

uint32_t send_buffer(int socket_fd, char* buffer, uint32_t buffer_length) {
    uint32_t nof_bytes = buffer_length;
    uint32_t sent_bytes;
    uint32_t total_nof_sent_bytes = 0;

    do {
        ASSERT((sent_bytes = send(socket_fd, buffer, nof_bytes, 0)) >= 0, "send failed");
        nof_bytes -= sent_bytes;
        buffer += sent_bytes;
        total_nof_sent_bytes += sent_bytes;
    } while (nof_bytes > 0);

    return total_nof_sent_bytes;
}

/*
* If RAND_MAX (the maximum value that rand can return) value is less then 2^16
* we need to get 2 random numbers, the first one will be the first 15 bits and
* the second one will be the 16th bit.
*/
int getRand() {
    int r, b;
    if (RAND_MAX >= MAX_RAND_VALUE) {
        return (rand() % RAND_MAX);
    }

    /* RAND_MAX = 2^15, need to do 2 rands*/
    r = rand();
    b = (rand() % 2); /* b is 0/1 */
    
    r += b << 15;
    return r;
}

int addNoise(char* msg, uint32_t msg_size, noiseType noise_type, int prob, int seed, int n) {
    int NumChangedBits = 0;
    char byte;
    uint8_t mask = 1 << 7;

    if (noise_type == randomy) {
        srand(seed);
        for (int i = 0; i < msg_size; i++) {
            byte = msg[i];
            mask = 1 << 7;
            for (int bit = 7; bit >= 0; bit--) {
                if (getRand() < prob) {
                    byte = byte ^ mask;
                    NumChangedBits++;
                }
                mask = mask >> 1;
            }
            msg[i] = byte;
        }
    }
    else { // noise_type == deterministic
        int cnt = 1;
        for (int i = 0; i < msg_size; i++) {
            byte = msg[i];
            mask = 1 << 7;
            for (int bit = 7; bit >= 0; bit--) {
                if (cnt == n) {
                    byte = byte ^ mask;
                    NumChangedBits++;
                    cnt = 0;
                }
                cnt++;
                mask = mask >> 1;
            }
            msg[i] = byte;
        }
    }
    return NumChangedBits;
}

int setup_listen_socket(char* type) {
    struct sockaddr_in serv_addr;
    char hostname[MAX_STR_LEN], *ipAddress;
    struct hostent* entry;
    int listen_fd;
    int size;

    ASSERT((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET, "socket failed");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(0);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ASSERT(bind(listen_fd, (SOCKADDR*)&serv_addr, sizeof(serv_addr)) == 0, "bind failed");
    ASSERT(listen(listen_fd, LISTEN_QUEUE_SIZE) == 0, "listen failed");

    /*get and print IP address and port number*/
    size = sizeof(serv_addr);
    ASSERT(getsockname(listen_fd, (struct sockaddr*)&serv_addr, &size) == NO_ERROR, "getsockname failed");
    ASSERT(gethostname(hostname, MAX_STR_LEN) == 0, "gethostname failed");
    entry = gethostbyname(hostname);
    ipAddress = inet_ntoa(*((struct in_addr*)entry->h_addr_list[0]));
    printf("%s socket: IP address = %s port = %d\n", type, ipAddress, ntohs(serv_addr.sin_port));

    return listen_fd;
}

int main(int argc, char* argv[]) {
    char buffer[ENC_BUFFER_LENGTH] = { 0 };
    char user_input[MAX_STR_LEN];
    struct sockaddr_in peer_addr;
    int addrsize = sizeof(struct sockaddr_in);
    int sender_listen_fd, receiver_listen_fd;
    int sender_fd, receiver_fd;
    uint16_t recieved_bytes, total_recieved_bytes, numChangedBits, totalNumChangedBits;
    WSADATA wsaData;

    ASSERT(WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR, "WSAStartup failed");

    int prob = -1;
    int seed = -1;
    int n = -1;
    noiseType noise_type;

    ASSERT(argc == 3 || argc == 4, "argc value is to big / small");

    char* noise_type_str = argv[1]; // Assuming that type of noise will be set as first argument 
    if (strcmp(noise_type_str, "-r") == 0) {
        ASSERT(argc == 4, "Invalid number of args");
        prob = atoi(argv[2]);
        seed = atoi(argv[3]);
        noise_type = randomy;
    }
    else if (strcmp(noise_type_str, "-d") == 0) {
        ASSERT(argc == 3, "Invalid number of args");
        n = atoi(argv[2]);
        noise_type = deterministic;
    }
    else {
        ASSERT(0, "Invalid noise type, use -r for random and -d of args");
    }

    sender_listen_fd = setup_listen_socket("sender");
    receiver_listen_fd = setup_listen_socket("receiver");

    while (1) {
        ASSERT((sender_fd = accept(sender_listen_fd, (SOCKADDR*)&peer_addr, &addrsize)) != INVALID_SOCKET, "accept sender failed");
        ASSERT((receiver_fd = accept(receiver_listen_fd, (SOCKADDR*)&peer_addr, &addrsize)) != INVALID_SOCKET, "accept receiver failed");

        recieved_bytes = 0;
        total_recieved_bytes = 0;
        numChangedBits = 0;
        totalNumChangedBits = 0;
        while (1) {
            recieved_bytes = get_buffer(sender_fd, buffer, ENC_BUFFER_LENGTH);
            total_recieved_bytes += recieved_bytes;
            if (recieved_bytes == 0) {
                break;
            }
            numChangedBits = addNoise(buffer, recieved_bytes, noise_type, prob, seed, n);
            totalNumChangedBits += numChangedBits;
            send_buffer(receiver_fd, buffer, recieved_bytes);
        }

        printf("retransmitted %d bytes, flipped %d bits\n", total_recieved_bytes, totalNumChangedBits);

        closesocket(sender_fd);
        closesocket(receiver_fd);

        printf("continue? (yes/no)\n");
        scanf("%s", user_input);
        if (strcmp("yes", user_input) == 0) {
            continue;
        }
        else if (strcmp("no", user_input) == 0) {
            break;
        }
        else {
            ASSERT(0, "Invalid answer - yes to continue, no to quit");
        }

    }
    closesocket(sender_listen_fd);
    closesocket(receiver_listen_fd);

    exit(0);
}