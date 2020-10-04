#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <time.h>
#include "socket.h"
#include "message.h"
#include "controller.h"
#include <sys/time.h>

#define MAXFD(x, y) ((x) >= (y)) ? (x) : (y)

int accept_sensor(int fd, int *device_record) {
    int sensor_index = 0;
    while (sensor_index < MAXDEV && device_record[sensor_index] != 0) {
        sensor_index++;
    }

    if (sensor_index == MAXDEV) {
        fprintf(stderr, "gateway: max concurrent connections\n");
        exit(-1);
    }

    return accept_connection(fd);
}

void read_from(int fd, char *cig_serialized, struct cignal *cig) {
    int num_read = read(fd, cig_serialized, CIGLEN);
    if (num_read == 0 || num_read != CIGLEN) {
        perror("gateway: read from sensor");
        exit(1);
    } else {
        // in case we don't have null terminator
        char to_print[CIGLEN + 1];
        strncpy(to_print, cig_serialized, CIGLEN);
        to_print[CIGLEN] = '\0';

        printf("RAW MESSAGE: %s\n", to_print);
        unpack_cignal(cig_serialized, cig);
    }
}

int main(int argc, char *argv[]) {
    int port;
    struct cignal cig;
    // A buffer to store a serialized message
    char *cig_serialized = malloc(sizeof(char) * CIGLEN);
    // An array to registered sensor devices
    int device_record[MAXDEV] = {0};

    if (argc == 2) {
        port = strtol(argv[1], NULL, 0);
    } else {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int gatewayfd = set_up_server_socket(port);
    printf("\nThe Cignal Gateway is now started on port: %d\n\n", port);
    int peerfd;

    /* TODO: Implement the body of the server.
     *
     * Use select so that the server process never blocks on any call except
     * select. If no sensors connect and/or send messsages in a timespan of
     * 5 seconds then select will return and print the message "Waiting for
     * Sensors update..." and go back to waiting for an event.
     *
     * The server will handle connections from devices, will read a message from
     * a sensor, process the message (using process_message), write back
     * a response message to the sensor client, and close the connection.
     * After reading a message, your program must print the "RAW MESSAGE"
     * message below, which shows the serialized message received from the *
     * client.
     *
     *  Print statements you must use:
     * 	printf("Waiting for Sensors update...\n");
     * 	printf("RAW MESSAGE: %s\n", YOUR_VARIABLE);
     */

    // TODO implement select loop
    int max_fd = gatewayfd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(gatewayfd, &all_fds);


    while (1) {
        char *serialized_cig;
        fd_set listen_fds = all_fds;
        struct timeval select_interval;
        select_interval.tv_sec = 5;
        select_interval.tv_usec = 0;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, &select_interval);
        if (nready == -1) {
            perror("gateway: select");
            exit(1);
        } else if (nready == 0) {
            printf("Waiting for Sensors update...\n");
        } else {
            if (FD_ISSET(gatewayfd, &listen_fds)) {

                peerfd = accept_sensor(gatewayfd, device_record);
//            if (peefd > max_fd) {
//                max_fd = sensor_fd;
//            }
                max_fd = MAXFD(max_fd, peerfd);

                FD_SET(peerfd, &all_fds);
                printf("Accepted connection\n");
            }

            if (FD_ISSET(peerfd, &listen_fds)) {
                // read serialized cignal from sensor socket and unpack it to cig
                read_from(peerfd, cig_serialized, &cig);
                //process cig
                if (process_message(&cig, device_record) != -1) {
                    //sent back the response cig
                    serialized_cig = serialize_cignal(cig);
                    if (write(peerfd, serialize_cignal(cig), CIGLEN) != CIGLEN) {
                        fprintf(stderr, "gateway: write back to sensor");
                        exit(1);
                    }
                } else {
                    exit(1);
                }

                //clear the non-useful peerfd in all_fds?
                FD_CLR(peerfd, &all_fds);

                //close connection
                close(peerfd);
                free(serialized_cig);

            }
        }
    }

    // Suppress unused variable warning.  The next 5 ilnes can be removed
    // after the variables are used.
    (void) gatewayfd;
    (void) peerfd;
    (void) cig;
    (void) device_record;
    (void) cig_serialized;
    return 0;
}
