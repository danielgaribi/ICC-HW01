/* gcc .\Receiver.c -lwsock32 -o .\Receiver.exe */
#include "receiver.h"

int decode_haming(short* msg_enc, short* msg_dec) {
    short C1, C2, C3, C4, C5;
    int data_index = 0;
    int err_index = 0;

    /* Calculate parity bits */
    /* Calculate C1 - bits: 0101 0101 0101 0101 0101 0101 0101 0101 */
    C1 = msg_enc[0] ^ msg_enc[2] ^ msg_enc[4] ^ msg_enc[6] ^ msg_enc[8] ^ msg_enc[10]
        ^ msg_enc[12] ^ msg_enc[14] ^ msg_enc[16] ^ msg_enc[18] ^ msg_enc[20]
        ^ msg_enc[22] ^ msg_enc[24] ^ msg_enc[26] ^ msg_enc[28] ^ msg_enc[30];

    /* Calculate C2 - bits: 0110 0110 0110 0110 0110 0110 0110 0110 */
    C2 = msg_enc[1] ^ msg_enc[2] ^ msg_enc[5] ^ msg_enc[6] ^ msg_enc[9] ^ msg_enc[10]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[17] ^ msg_enc[18] ^ msg_enc[21]
        ^ msg_enc[22] ^ msg_enc[25] ^ msg_enc[26] ^ msg_enc[29] ^ msg_enc[30];

    /* Calculate C3 - bits: 0111 1000 0111 1000 0111 1000 0111 1000 */
    C3 = msg_enc[3] ^ msg_enc[4] ^ msg_enc[5] ^ msg_enc[6] ^ msg_enc[11] ^ msg_enc[12]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[19] ^ msg_enc[20] ^ msg_enc[21]
        ^ msg_enc[22] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];

    /* Calculate C4 - bits: 0111 1111 1000 0000 0111 1111 1000 0000 */
    C4 = msg_enc[7] ^ msg_enc[8] ^ msg_enc[9] ^ msg_enc[10] ^ msg_enc[11] ^ msg_enc[12]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[23] ^ msg_enc[24] ^ msg_enc[25]
        ^ msg_enc[26] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];

    /* Calculate C5 - bits: 0111 1111 1111 1111 1000 0000 0000 0000 */
    C5 = msg_enc[15] ^ msg_enc[16] ^ msg_enc[17] ^ msg_enc[18] ^ msg_enc[19] ^ msg_enc[20]
        ^ msg_enc[21] ^ msg_enc[22] ^ msg_enc[23] ^ msg_enc[24] ^ msg_enc[25]
        ^ msg_enc[26] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];

    /* Fix msg if needed */
    err_index += C1_WEIGHT * C1 + C2_WEIGHT * C2 + C3_WEIGHT * C3 + C4_WEIGHT * C4 + C5_WEIGHT * C5;
    if (err_index != 0) {
        msg_enc[err_index - 1] = (msg_enc[err_index - 1] == 1) ? 0 : 1; // Flip bit 
    } // Else - error not detected 

    /* Extract data from encoded msg */
    for (int i = 0; i < FRAME_NOF_BITS; i++) {
        if ((i == C1_INDEX) || (i == C2_INDEX) || (i == C3_INDEX) || (i == C4_INDEX) || (i == C5_INDEX)) {
            continue;
        }
        else {
            msg_dec[data_index] = msg_enc[i];
            data_index++;
        }
    }

    if (err_index == 0) {
        return 0;
    }
    else {
        return 1;
    }
}

uint32_t get_buffer(int socket_fd, char* enc_buffer, uint32_t buffer_length) {
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

    } while (nof_bytes_rec > 0);

    return total_nof_bytes_rec;
}

uint32_t write_file(FILE* file_fd, uint32_t buffer_length, char* buffer) {
    uint32_t nof_write_bytes = 0;
    ASSERT((nof_write_bytes = fwrite(buffer, 1, buffer_length, file_fd)) == buffer_length, "fwrite failed");
    return nof_write_bytes;
}

void convert_bit_array_to_buffer(char* buffer, int* buffer_length, short* bits_array, int bits_array_length) {
    int i, j, index = 0;
    short offset;
    char c;
    int size_of_char_in_bits = CONVERT_BYTES_TO_BITS(sizeof(char));

    c = 0;
    offset = size_of_char_in_bits - 1;
    memset(buffer, 0, ENC_BUFFER_LENFTH);
    for (i = 0; i < bits_array_length; i++) {
        c = c | (bits_array[i] << offset);
        offset--;
        if (i % size_of_char_in_bits == (size_of_char_in_bits - 1)) {
            buffer[index] = c;
            index++;
            c = 0;
            offset = size_of_char_in_bits - 1;
        }
    }

    *buffer_length = index;
}

uint32_t dec_file(char* buffer, int* buffer_length, char* enc_buffer, int enc_buffer_length) {
    short data_bits_array[FRAME_NOF_DATA_BITS * NOF_FRAMS_IN_PACKET] = { 0 };
    short enc_bits_array[FRAME_NOF_BITS] = { 0 };
    short* sub_data_bits_array = data_bits_array;
    int size_of_char_in_bits = CONVERT_BYTES_TO_BITS(sizeof(char));
    short mask;
    int i, j, index = 0;
    uint32_t nof_corrected_bytes = 0, nof_bits = 0;
    for (i = 0; i < enc_buffer_length; i++) {
        for (j = size_of_char_in_bits - 1; j >= 0; j--) {
            printf("string: %u\n", enc_buffer[i]);
            printf("index: %u\n", j);
            printf("value: %u\n", (enc_buffer[i] >> j) & 1);
            enc_bits_array[index] = (enc_buffer[i] >> j) & 1;
            index++;
            if (index == FRAME_NOF_BITS) {
                printf("before H:\n");
                for (int k = 0; k < FRAME_NOF_BITS; k++) {
                    printf("%d: %u\n", k, enc_bits_array[k]);
                }
                nof_corrected_bytes += decode_haming(enc_bits_array, sub_data_bits_array);
                printf("After H:\n");
                for (int k = 0; k < FRAME_NOF_DATA_BITS; k++) {
                    printf("%d: %u\n", k, sub_data_bits_array[k]);
                }
                nof_bits += FRAME_NOF_DATA_BITS;
                sub_data_bits_array += FRAME_NOF_DATA_BITS;
                index = 0;
            }
        }
    }
    for (int k = 0; k < enc_buffer_length * NOF_BITS_IN_BYTE; k++) {
        printf("%d: %u\n", k, data_bits_array[k]);
    }
    convert_bit_array_to_buffer(buffer, buffer_length, data_bits_array, nof_bits);
    return nof_corrected_bytes;
}

int main(int argc, char* argv[]) {
    char* server_ip, file_path[MAX_STR_LEN];
    int server_port;
    int socket_fd;
    FILE* file_fd;
    uint32_t data_buffer_length, nof_corrected_bytes, nof_write_bytes, recieved_bytes, total_recieved_bytes;
    struct sockaddr_in serv_addr;
    WSADATA wsaData;

    char data_buffer[DATA_BUFFER_LENFTH] = { 0 };
    char enc_buffer[ENC_BUFFER_LENFTH] = { 0 };

    ASSERT(WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR, "WSAStartup failed");

    ASSERT(argc == 3, "argc value is to big / small");

    server_ip = argv[1];
    server_port = atoi(argv[2]);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    serv_addr.sin_addr.s_addr = inet_addr(server_ip);


    while (1) {
        /* Create TCP Socket */
        ASSERT((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET, "socket failed");
        ASSERT(connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != SOCKET_ERROR, "connect failed");

        printf("enter file name:\n");
        scanf_s("%s", file_path);

        if (strcmp(file_path, "quit") == 0) {
            closesocket(socket_fd);
            break;
        }

        ASSERT(fopen_s(&file_fd, file_path, "wb") == 0, "open failed");

        recieved_bytes = 0;
        total_recieved_bytes = 0;
        nof_corrected_bytes = 0;
        nof_write_bytes = 0;
        while (1) {
            recieved_bytes = get_buffer(socket_fd, enc_buffer, NOF_FRAMS_IN_PACKET * FRAME_NOF_BITS);
            total_recieved_bytes += recieved_bytes;
            if (recieved_bytes == 0) {
                break;
            }
            nof_corrected_bytes += dec_file(data_buffer, &data_buffer_length, enc_buffer, recieved_bytes);
            for (int k = 0; k < data_buffer_length; k++) {
                printf("%d: %u\n", k, data_buffer[k]);
            }
            nof_write_bytes += write_file(file_fd, data_buffer_length, data_buffer);
        }

        printf("wrote: %u bytes\n", nof_write_bytes);
        printf("corrected %u errors\n", nof_corrected_bytes);

        fclose(file_fd);
        closesocket(socket_fd);
    }

    exit(0);
}

