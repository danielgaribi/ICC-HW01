#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#define SUCCESS         0
#define FAILURE         1
#define FALSE           0
#define TRUE            1

typedef enum noiseType {
    random  = 0,
    deterministic  = 1,
    noNoise = 2, // debug: For debug 
} noiseType;


char* addNoise( char *msg, noiseType noise_type, int prob, int seed, int n, int *NumChangedBits ) {
    char *NoisyMsg = " "; // debug: set for compilition to pass   
    if ( noise_type == random ) {

    } else if ( noise_type == deterministic ) {

    } else { // noNoise 
        *NumChangedBits = 0;
        return msg;
    } 
    return NoisyMsg; 
}

int main( int argc, char *argv[] ) { 
    int ret;
    int prob            = -1;
    int seed            = -1; 
    int n               = -1;
    int NumChangedBits  = -1;
    char *answer = ""; // debug: set for code to compile 
    noiseType noise_type;

    if ( argc < 2 && argc > 3 ) {
        fprintf( stderr, "Invalid number of args. Error: %s\n", strerror( errno ) );
        exit( FAILURE );
    }

    char *noise_type_str = argv[ 1 ]; // Assuming that type of noise will be set as first argument 
    if ( strcmp( noise_type_str, "-r") == 0 ) {
        if ( argc != 3 ) {
            fprintf( stderr, "Invalid number of args. Error: %s\n", strerror( errno ) );
            exit( FAILURE );
        }
        prob = atoi( argv[ 2 ] );
        seed = atoi( argv[ 3 ] );
        noise_type = random;
    } else if ( strcmp( noise_type_str, "-d") == 0 ) {
        if ( argc != 2 ) {
            fprintf( stderr, "Invalid number of args. Error: %s\n", strerror( errno ) );
            exit( FAILURE );
        }
        n = atoi( argv[ 2 ] );
        noise_type = deterministic;
    } else if ( strcmp( noise_type_str, "-n") == 0 ) { // debug: For debug 
        noise_type = noNoise; 
    } else {
        fprintf( stderr, "Invalid noise type, use -r for random and -d of args. Error: %s\n", strerror( errno ) );
        exit( FAILURE );
    }

    /* Open sockets with Sender and Reciver as Server */


    /* Print IP, Port */
    char *Sender_IP_addr = " "; // debug: char *Sender_IP_addr = inet_ntoa(in_addr); // Garibi: set arg as struct in_addr
    int Sender_port = 0; // Garibi: set socket port
    char *Receiver_IP_addr = " "; // debug: char *Receiver_IP_addr = inet_ntoa(in_addr); // Garibi: set arg as struct in_addr
    int Receiver_port = 0; // Garibi: set socket port

    printf("sender socket: IP address = %s port = %d\n", Sender_IP_addr, Sender_port); 
    printf("receiver socket: IP address = %s port = %d\n", Receiver_IP_addr, Receiver_port); 

    /* Get msg from Sender */


    /* Add noise to msg */
    char *msg = " "; // debug: set msg 
    char *NoisedMsg = addNoise( msg, noise_type, prob, seed, n, &NumChangedBits ); 

    int msg_size = 0; // debug: set msg size after padding of haming code 
    printf("retransmitted %d bytes, flipped %d bits", msg_size, NumChangedBits);

    /* Send noides msg */ 

    /* ask user if to continue */
    char *buffer = malloc(4 * sizeof(char));
    if ( buffer == NULL ) {
        fprintf( stderr, "malloc failed. Error: %s\n", strerror( errno ) );
        exit( FAILURE );
    }

    while ( TRUE ) {
        // debug: assume you set this code in a loop... 
        printf("continue? (yes/no)");
        ret = sscanf(buffer, "%s", answer);
        if ( strcmp("yes", answer) == 0 ) {
            continue;
        } else if (strcmp( "no", answer ) ) {
            break;
        } else {
            fprintf( stderr, "Invalid answer - yes to continue, no to quit. Error: %s\n", strerror( errno ) );
            exit( FAILURE );
        }
    }

    printf("%s %d\n", NoisedMsg, ret); // debug: delete, add for compile to pass 

    free(buffer);
    return 0;
}
