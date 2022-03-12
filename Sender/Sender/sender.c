/* gcc .\Sender.c -lwsock32 -o .\Sender.exe */

#include "sender.h"

void encode_haming(short* msg) {
    short msg_enc[FRAME_NOF_BITS] = { 0 };
    short C1, C2, C3, C4, C5;
    int data_index = 0;

    /* Set data bits in hamming index, set parity bits 0 for now */
    for (int i = 0; i < FRAME_NOF_BITS; i++) {
        if ((i == C1_INDEX) || (i == C2_INDEX) || (i == C3_INDEX) || (i == C4_INDEX) || (i == C5_INDEX)) {
            msg_enc[i] = 0;
        }
        else {
            msg_enc[i] = msg[data_index];
            data_index++;
        }
    }

    /* Calculate parity bits */
    C1 = msg_enc[0] ^ msg_enc[2] ^ msg_enc[4] ^ msg_enc[6] ^ msg_enc[8] ^ msg_enc[10]
        ^ msg_enc[12] ^ msg_enc[14] ^ msg_enc[16] ^ msg_enc[18] ^ msg_enc[20]
        ^ msg_enc[22] ^ msg_enc[24] ^ msg_enc[26] ^ msg_enc[28] ^ msg_enc[30];
    msg_enc[0] = C1;

    /* Calculate C2 */
    C2 = msg_enc[1] ^ msg_enc[2] ^ msg_enc[5] ^ msg_enc[6] ^ msg_enc[9] ^ msg_enc[10]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[17] ^ msg_enc[18] ^ msg_enc[21]
        ^ msg_enc[22] ^ msg_enc[25] ^ msg_enc[26] ^ msg_enc[29] ^ msg_enc[30];
    msg_enc[1] = C2;

    /* Calculate C3 */
    C3 = msg_enc[3] ^ msg_enc[4] ^ msg_enc[5] ^ msg_enc[6] ^ msg_enc[11] ^ msg_enc[12]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[19] ^ msg_enc[20] ^ msg_enc[21]
        ^ msg_enc[22] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];
    msg_enc[3] = C3;

    /* Calculate C4 */
    C4 = msg_enc[7] ^ msg_enc[8] ^ msg_enc[9] ^ msg_enc[10] ^ msg_enc[11] ^ msg_enc[12]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[23] ^ msg_enc[24] ^ msg_enc[25]
        ^ msg_enc[26] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];
    msg_enc[7] = C4;

    /* Calculate C5 */
    C5 = msg_enc[15] ^ msg_enc[16] ^ msg_enc[17] ^ msg_enc[18] ^ msg_enc[19] ^ msg_enc[20]
        ^ msg_enc[21] ^ msg_enc[22] ^ msg_enc[23] ^ msg_enc[24] ^ msg_enc[25]
        ^ msg_enc[26] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];
    msg_enc[15] = C5;

    /* Copy back into original array */
    for (int i = 0; i < 31; i++) {
        msg[i] = msg_enc[i];
    }
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

int get_file_size(FILE* file_fd) {
    int file_length;
    fseek(file_fd, 0L, SEEK_END);
    file_length = ftell(file_fd);
    fseek(file_fd, 0, SEEK_SET);
    // rewind(file_fd);
    return file_length;
}

int read_frame_from_file(FILE* file_fd, int file_length, char* buffer) {
    int nof_read_bytes = 0;
    int nof_remain_bytes = min(CONVERT_BIT_TO_BYTES(FRAME_NOF_DATA_BITS * NOF_FRAMS_IN_PACKET), file_length);
    int total_nof_read_bytes = 0;
    char* working_buffer = buffer;

    memset(buffer, 0, DATA_BUFFER_LENFTH);
    while ((nof_read_bytes = fread(working_buffer, sizeof(char), nof_remain_bytes, file_fd)) != nof_remain_bytes) {
        nof_remain_bytes -= nof_read_bytes;
        working_buffer += nof_read_bytes;
        total_nof_read_bytes += nof_read_bytes;
    }
    total_nof_read_bytes += nof_read_bytes;
    return total_nof_read_bytes;
}

void convert_bit_array_to_buffer(char* enc_buffer, int* enc_buffer_length, short* bits_array, int bits_array_length) {
    int i, j, index = 0;
    int is_during_word = 0;
    short offset;
    char c;
    int size_of_char_in_bits = CONVERT_BYTES_TO_BITS(sizeof(char));

    c = 0;
    offset = size_of_char_in_bits - 1;
    memset(enc_buffer, 0, ENC_BUFFER_LENFTH);
    for (i = 0; i < bits_array_length; i++) {
        c = c | (bits_array[i] << offset);
        offset--;
        if (i % size_of_char_in_bits == (size_of_char_in_bits - 1)) {
            enc_buffer[index] = c;
            index++;
            c = 0;
            offset = size_of_char_in_bits - 1;
            is_during_word = 0;
        }
        else {
            is_during_word = 1;
        }
    }

    if (is_during_word == 1) {
        enc_buffer[index] = c;
        index++;
    }

    *enc_buffer_length = index;
}

void enc_file(char* buffer, uint32_t buffer_length, char* enc_buffer, uint32_t* enc_buffer_length) {
    short bits_array[FRAME_NOF_BITS * NOF_FRAMS_IN_PACKET] = { 0 };
    short* sub_bits_array = bits_array;
    int size_of_char_in_bits = CONVERT_BYTES_TO_BITS(sizeof(char));
    int i, j, index = 0;
    uint32_t nof_bits = 0;
    for (i = 0; i < buffer_length; i++) {
        for (j = size_of_char_in_bits - 1; j >= 0; j--) {
            sub_bits_array[index] = (buffer[i] >> j) & 1;
            index++;
            if (index == FRAME_NOF_DATA_BITS) {
                encode_haming(sub_bits_array);
                sub_bits_array += FRAME_NOF_BITS;
                nof_bits += FRAME_NOF_BITS;
                index = 0;
            }
        }
    }
    if (index != 0) {
        encode_haming(sub_bits_array);
        sub_bits_array += FRAME_NOF_BITS;
        nof_bits += FRAME_NOF_BITS;
    }
    convert_bit_array_to_buffer(enc_buffer, enc_buffer_length, bits_array, nof_bits);
}

int main(int argc, char* argv[]) {
    char* server_ip, file_path[MAX_STR_LEN];
    int server_port;
    int socket_fd;
    FILE* file_fd;
    uint32_t file_length, enc_buffer_length, read_bytes, sent_bytes, total_read_bytes, remain_bytes;
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
        ASSERT((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) != 0, "socket failed");
        ASSERT(connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) >= 0, "connect failed");

        printf("enter file name:\n");
        scanf("%s", file_path);

        if (strcmp(file_path, "quit") == 0) {
            closesocket(socket_fd);
            break;
        }

        ASSERT(fopen_s(&file_fd, file_path, "rb") == 0, "open failed");

        /* Get and print file length */
        file_length = get_file_size(file_fd);
        printf("file length: %u bytes\n", file_length);

        read_bytes = 0;
        sent_bytes = 0;
        total_read_bytes = 0;
        remain_bytes = file_length;
        /* Read file and send */
        while (file_length != total_read_bytes) {
            read_bytes = read_frame_from_file(file_fd, remain_bytes, data_buffer);
            remain_bytes -= read_bytes;
            total_read_bytes += read_bytes;
            enc_file(data_buffer, read_bytes, enc_buffer, &enc_buffer_length);
            sent_bytes += send_buffer(socket_fd, enc_buffer, enc_buffer_length);
        }

        printf("sent: %u bytes\n", sent_bytes);

        fclose(file_fd);
        closesocket(socket_fd);
    }

    exit(0);
}
