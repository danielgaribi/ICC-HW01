#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define FRAME_NOF_BITS  31
#define TRUE            1
#define FALSE           0

#define C1_INDEX                0
#define C2_INDEX                1
#define C3_INDEX                3
#define C4_INDEX                7
#define C5_INDEX                15

#define C1_WEIGHT               1
#define C2_WEIGHT               2
#define C3_WEIGHT               4
#define C4_WEIGHT               8
#define C5_WEIGHT               16

void printMsg(short* msg) {
    printf("msg:\t");
    int cnt = 1;
    for (int i = 0; i < FRAME_NOF_BITS; i++) {
        printf("%d", (int)msg[i]);
        if (cnt == 4) {
            printf(" ");
            cnt = 0;
        }
        cnt++;
    }
    printf("\n");
}

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
    msg_enc[C1_INDEX] = C1;

    /* Calculate C2 */
    C2 = msg_enc[1] ^ msg_enc[2] ^ msg_enc[5] ^ msg_enc[6] ^ msg_enc[9] ^ msg_enc[10]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[17] ^ msg_enc[18] ^ msg_enc[21]
        ^ msg_enc[22] ^ msg_enc[25] ^ msg_enc[26] ^ msg_enc[29] ^ msg_enc[30];
    msg_enc[C2_INDEX] = C2;

    /* Calculate C3 */
    C3 = msg_enc[3] ^ msg_enc[4] ^ msg_enc[5] ^ msg_enc[6] ^ msg_enc[11] ^ msg_enc[12]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[19] ^ msg_enc[20] ^ msg_enc[21]
        ^ msg_enc[22] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];
    msg_enc[C3_INDEX] = C3;

    /* Calculate C4 */
    C4 = msg_enc[7] ^ msg_enc[8] ^ msg_enc[9] ^ msg_enc[10] ^ msg_enc[11] ^ msg_enc[12]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[23] ^ msg_enc[24] ^ msg_enc[25]
        ^ msg_enc[26] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];
    msg_enc[C4_INDEX] = C4;

    /* Calculate C5 */
    C5 = msg_enc[15] ^ msg_enc[16] ^ msg_enc[17] ^ msg_enc[18] ^ msg_enc[19] ^ msg_enc[20]
        ^ msg_enc[21] ^ msg_enc[22] ^ msg_enc[23] ^ msg_enc[24] ^ msg_enc[25]
        ^ msg_enc[26] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];
    msg_enc[C5_INDEX] = C5;

    /* Copy back into original array */
    for (int i = 0; i < 31; i++) {
        msg[i] = msg_enc[i];
    }
}

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

    /* Calculate C4 - bits: 0000 0001 1111 1110 0000 0001 1111 111 */
    C4 = msg_enc[7] ^ msg_enc[8] ^ msg_enc[9] ^ msg_enc[10] ^ msg_enc[11] ^ msg_enc[12]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[23] ^ msg_enc[24] ^ msg_enc[25]
        ^ msg_enc[26] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];

    printMsg(msg_enc);
    printf("msg_enc[7]: %u\n", msg_enc[7]);
    printf("msg_enc[8]: %u\n", msg_enc[8]);
    printf("msg_enc[9]: %u\n", msg_enc[9]);
    printf("msg_enc[10]: %u\n", msg_enc[10]);
    printf("msg_enc[11]: %u\n", msg_enc[11]);
    printf("msg_enc[12]: %u\n", msg_enc[12]);
    printf("msg_enc[13]: %u\n", msg_enc[13]);
    printf("msg_enc[14]: %u\n", msg_enc[14]);

    printf("msg_enc[23]: %u\n", msg_enc[23]);
    printf("msg_enc[24]: %u\n", msg_enc[24]);
    printf("msg_enc[25]: %u\n", msg_enc[25]);
    printf("msg_enc[26]: %u\n", msg_enc[26]);
    printf("msg_enc[27]: %u\n", msg_enc[27]);
    printf("msg_enc[28]: %u\n", msg_enc[28]);
    printf("msg_enc[29]: %u\n", msg_enc[29]);
    printf("msg_enc[30]: %u\n", msg_enc[30]);

    printf("xor C4: %u\n", msg_enc[7] ^ msg_enc[8] ^ msg_enc[9] ^ msg_enc[10] ^ msg_enc[11] ^ msg_enc[12]
        ^ msg_enc[13] ^ msg_enc[14] ^ msg_enc[23] ^ msg_enc[24] ^ msg_enc[25]
        ^ msg_enc[26] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30]);


    /* Calculate C5 - bits: 0000 0000 0000 0000 1111 1111 1111 1111 */
    C5 = msg_enc[15] ^ msg_enc[16] ^ msg_enc[17] ^ msg_enc[18] ^ msg_enc[19] ^ msg_enc[20]
        ^ msg_enc[21] ^ msg_enc[22] ^ msg_enc[23] ^ msg_enc[24] ^ msg_enc[25]
        ^ msg_enc[26] ^ msg_enc[27] ^ msg_enc[28] ^ msg_enc[29] ^ msg_enc[30];

    /* Fix msg if needed */
    err_index += C1_WEIGHT * C1 + C2_WEIGHT * C2 + C3_WEIGHT * C3 + C4_WEIGHT * C4 + C5_WEIGHT * C5;
    printf("\ndecode_haming()\n");
    printf("C5: %u\tC4: %u\tC3: %u\tC2: %u\tC1: %u\n", C5, C4, C3, C2, C1);
    printf("err_index = %d\n\n", err_index);

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

int MsgCmp(short* msg1, short* msg2) {
    int result = TRUE;
    for (int i = 0; i < FRAME_NOF_BITS; i++) {
        if (msg1[i] != msg2[i]) {
            result = FALSE;
            break;
        }
    }
    if (result == FALSE) {
        printf("msg are not identical!\n");
        printf("msg1: \t");
        printMsg(msg1);
        printf("msg2: \t");
        printMsg(msg2);
    }
    return result;
}

int test_all_zeros() {
    short msg[FRAME_NOF_BITS] = { 0 };
    short msg_dec[FRAME_NOF_BITS] = { 0 };
    short msg_enc_res[FRAME_NOF_BITS] = { 0 };
    short msg_dec_res[FRAME_NOF_BITS] = { 0 };

    // Test encoding (inplace)
    encode_haming(msg);
    if (!MsgCmp(msg, msg_enc_res)) {
        printf("test all zeros - failed encoding!\n");
        return FALSE;
    }
    else {
        printf("test all zeros - pass encoding!\n");
    }

    // Test decoding (not inplace)
    int error_index = decode_haming(msg, msg_dec);
    if (!MsgCmp(msg_dec, msg_dec_res)) {
        printf("test all zeros - failed decoding!\n");
        return FALSE;
    }
    else if (error_index != 0) {
        printf("test all zeros - error_index != 0!\n");
        return FALSE;
    }
    else {
        printf("test all zeros - pass decoding!\n");
        return TRUE;
    }
}

int test_all_zeros_fix_bit(int bit_flip_index) {
    short msg[FRAME_NOF_BITS] = { 0 };
    short msg_dec[FRAME_NOF_BITS] = { 0 };
    short msg_enc_res[FRAME_NOF_BITS] = { 0 };
    short msg_dec_res[FRAME_NOF_BITS] = { 0 };

    // Test encoding (inplace)
    encode_haming(msg);
    if (!MsgCmp(msg, msg_enc_res)) {
        printf("test all zeros fix bit - failed encoding!\n");
        return FALSE;
    }
    else {
        printf("test all zeros fix bit - pass encoding!\n");
    }

    // flip bit in index bit_flip_index
    msg[bit_flip_index] = (msg[bit_flip_index] == 0) ? 1 : 0;

    // Test decoding (not inplace)
    int error_index = decode_haming(msg, msg_dec);
    if (!MsgCmp(msg_dec, msg_dec_res)) {
        printf("test all zeros fix bit - failed decoding!\n");
        return FALSE;
    }
    else if (error_index != 1) {
        printf("test all zeros fix bit - error_index != 1!\n");
        return FALSE;
    }
    else {
        printf("test all zeros fix bit - pass decoding!\n");
        return TRUE;
    }
}


// msg:     0110 0001 0110 0010 0110 0011 01
// msg_enc: 1100 1101 0001 0110 0001 0011 0001 101
int test1() {
    short msg[FRAME_NOF_BITS] = { 0 };
    msg[1] = 1;
    msg[2] = 1;
    msg[7] = 1;
    msg[9] = 1;
    msg[10] = 1;
    msg[14] = 1;
    msg[17] = 1;
    msg[18] = 1;
    msg[22] = 1;
    msg[23] = 1;
    msg[25] = 1;

    short msg_dec[FRAME_NOF_BITS] = { 0 };

    // msg: 1100 1101 0001 0110 0001 0011 0001 101
    short msg_enc_res[FRAME_NOF_BITS] = { 0 };
    msg_enc_res[0] = 1;
    msg_enc_res[1] = 1;
    msg_enc_res[4] = 1;
    msg_enc_res[5] = 1;
    msg_enc_res[7] = 1;
    msg_enc_res[11] = 1;
    msg_enc_res[13] = 1;
    msg_enc_res[14] = 1;
    msg_enc_res[19] = 1;
    msg_enc_res[22] = 1;
    msg_enc_res[23] = 1;
    msg_enc_res[27] = 1;
    msg_enc_res[28] = 1;
    msg_enc_res[30
    ] = 1;

    short msg_dec_res[FRAME_NOF_BITS] = { 0 };
    // Copy msg into msg_dec_res
    for (int i = 0; i < FRAME_NOF_BITS; i++) {
        msg_dec_res[i] = msg[i];
    }

    // Test encoding (inplace)
    encode_haming(msg);
    if (!MsgCmp(msg, msg_enc_res)) {
        printf("test1 - failed encoding!\n");
        return FALSE;
    }
    else {
        printf("test1 - pass encoding!\n");
    }

    // Test decoding (not inplace)
    int error_index = decode_haming(msg, msg_dec);
    if (!MsgCmp(msg_dec, msg_dec_res)) {
        printf("test1 - failed decoding!\n");
        return FALSE;
    }
    else if (error_index != 0) {
        printf("test1 - error_index != 0!\n");
        return FALSE;
    }
    else {
        printf("test1 - pass decoding!\n");
        return TRUE;
    }
}

// msg:     0110 0001 0110 0010 0110 0011 01
// msg_enc: 1100 1101 0001 0110 0001 0011 0001 101
int test1_fix_bit(int bit_flip_index) {
    short msg[FRAME_NOF_BITS] = { 0 };
    msg[1] = 1;
    msg[2] = 1;
    msg[7] = 1;
    msg[9] = 1;
    msg[10] = 1;
    msg[14] = 1;
    msg[17] = 1;
    msg[18] = 1;
    msg[22] = 1;
    msg[23] = 1;
    msg[25] = 1;

    short msg_dec[FRAME_NOF_BITS] = { 0 };

    // msg: 1100 1101 0001 0110 0001 0011 0001 101
    short msg_enc_res[FRAME_NOF_BITS] = { 0 };
    msg_enc_res[0] = 1;
    msg_enc_res[1] = 1;
    msg_enc_res[4] = 1;
    msg_enc_res[5] = 1;
    msg_enc_res[7] = 1;
    msg_enc_res[11] = 1;
    msg_enc_res[13] = 1;
    msg_enc_res[14] = 1;
    msg_enc_res[19] = 1;
    msg_enc_res[22] = 1;
    msg_enc_res[23] = 1;
    msg_enc_res[27] = 1;
    msg_enc_res[28] = 1;
    msg_enc_res[30
    ] = 1;

    short msg_dec_res[FRAME_NOF_BITS] = { 0 };
    // Copy msg into msg_dec_res
    for (int i = 0; i < FRAME_NOF_BITS; i++) {
        msg_dec_res[i] = msg[i];
    }

    // Test encoding (inplace)
    encode_haming(msg);
    if (!MsgCmp(msg, msg_enc_res)) {
        printf("test1_fix_bit - failed encoding!\n");
        return FALSE;
    }
    else {
        printf("test1_fix_bit - pass encoding!\n");
    }

    // flip bit
    msg[bit_flip_index] = (msg[bit_flip_index] == 0) ? 1 : 0;

    // Test decoding (not inplace)
    int error_index = decode_haming(msg, msg_dec);
    if (!MsgCmp(msg_dec, msg_dec_res)) {
        printf("test1_fix_bit - failed decoding!\n");
        return FALSE;
    }
    else if (error_index != 1) {
        printf("test1_fix_bit - error_index != 1!\n");
        return FALSE;
    }
    else {
        printf("test1_fix_bit - pass decoding!\n");
        return TRUE;
    }
}

// msg:     1100 0000 1100 0000 1100 0000 11
// msg_enc: 1011 1000 0000 1100 0000 0110 0000 011
int test2() {
    short msg[FRAME_NOF_BITS] = { 0 };
    msg[0] = 1;
    msg[1] = 1;
    msg[8] = 1;
    msg[9] = 1;
    msg[16] = 1;
    msg[17] = 1;
    msg[24] = 1;
    msg[25] = 1;

    short msg_dec[FRAME_NOF_BITS] = { 0 };

    // msg: 1011 1000 0000 1100 0000 0110 0000 011
    short msg_enc_res[FRAME_NOF_BITS] = { 0 };
    msg_enc_res[0] = 1;
    msg_enc_res[2] = 1;
    msg_enc_res[3] = 1;
    msg_enc_res[4] = 1;
    msg_enc_res[12] = 1;
    msg_enc_res[13] = 1;
    msg_enc_res[21] = 1;
    msg_enc_res[22] = 1;
    msg_enc_res[29] = 1;
    msg_enc_res[30] = 1;

    short msg_dec_res[FRAME_NOF_BITS] = { 0 };
    // Copy msg into msg_dec_res
    for (int i = 0; i < FRAME_NOF_BITS; i++) {
        msg_dec_res[i] = msg[i];
    }

    // Test encoding (inplace)
    encode_haming(msg);
    if (!MsgCmp(msg, msg_enc_res)) {
        printf("test2 - failed encoding!\n");
        return FALSE;
    }
    else {
        printf("test2 - pass encoding!\n");
    }

    // Test decoding (not inplace)
    int error_index = decode_haming(msg, msg_dec);
    if (!MsgCmp(msg_dec, msg_dec_res)) {
        printf("test2 - failed decoding!\n");
        return FALSE;
    }
    else if (error_index != 0) {
        printf("test2 - error_index != 0!\n");
        return FALSE;
    }
    else {
        printf("test2 - pass decoding!\n");
        return TRUE;
    }
}

// msg:     
// msg_enc: 
int test2_fix_bit(int bit_flip_index) {
    short msg[FRAME_NOF_BITS] = { 0 };
    msg[0] = 1;
    msg[1] = 1;
    msg[8] = 1;
    msg[9] = 1;
    msg[16] = 1;
    msg[17] = 1;
    msg[24] = 1;
    msg[25] = 1;

    short msg_dec[FRAME_NOF_BITS] = { 0 };

    // msg: 
    short msg_enc_res[FRAME_NOF_BITS] = { 0 };
    msg_enc_res[0] = 1;
    msg_enc_res[2] = 1;
    msg_enc_res[3] = 1;
    msg_enc_res[4] = 1;
    msg_enc_res[12] = 1;
    msg_enc_res[13] = 1;
    msg_enc_res[21] = 1;
    msg_enc_res[22] = 1;
    msg_enc_res[29] = 1;
    msg_enc_res[30] = 1;

    short msg_dec_res[FRAME_NOF_BITS] = { 0 };
    // Copy msg into msg_dec_res
    for (int i = 0; i < FRAME_NOF_BITS; i++) {
        msg_dec_res[i] = msg[i];
    }

    // Test encoding (inplace)
    encode_haming(msg);
    if (!MsgCmp(msg, msg_enc_res)) {
        printf("test2_fix_bit - failed encoding!\n");
        return FALSE;
    }
    else {
        printf("test2_fix_bit - pass encoding!\n");
    }

    // flip bit
    msg[bit_flip_index] = (msg[bit_flip_index] == 0) ? 1 : 0;

    // Test decoding (not inplace)
    int error_index = decode_haming(msg, msg_dec);
    if (!MsgCmp(msg_dec, msg_dec_res)) {
        printf("test2_fix_bit - failed decoding!\n");
        return FALSE;
    }
    else if (error_index != 1) {
        printf("test2_fix_bit - error_index != 1!\n");
        return FALSE;
    }
    else {
        printf("test2_fix_bit - pass decoding!\n");
        return TRUE;
    }
}

int main() {
    //int res1 = test_all_zeros();
    //int res2 = 1;
    //for (int i = 0; i < FRAME_NOF_BITS; i++) {
    //    if (!test_all_zeros_fix_bit(i)) {
    //        res2 = 0;
    //        printf("test_all_zeros_fix_bit failed with i = %d\n", i);
    //        break;
    //    }
    //}

    //int res3 = test1();
    //int res4 = 1;
    //for (int i = 0; i < FRAME_NOF_BITS; i++) {
    //    if (!test1_fix_bit(i)) {
    //        res4 = 0;
    //        printf("test1_fix_bit failed with i = %d\n", i);
    //        break;
    //    }
    //}
    int res5 = test2();
    int res6 = 1;
    for (int i = 0; i < FRAME_NOF_BITS; i++) {
        if (!test2_fix_bit(i)) {
            res6 = 0;
            printf("test2_fix_bit failed with i = %d\n", i);
            break;
        }
    }

    //if (res1 + res2 + res3 + res4 + res5 + res6 == 6) {
    if (res5 + res6 == 2) {
        printf("passed all tests!\n");
    }
    else {
        printf("test failed... try again\n");
    }
    return 0;
}