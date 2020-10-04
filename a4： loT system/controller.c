#include <stdio.h>
#include "controller.h"
/* Checks if the message has a valid type. The gateway should only receive
 * HANDSHAKE or UPDATE messages from sensors
 */
int is_valid_type(struct cignal *cig) {
    if ((cig->hdr.type == HANDSHAKE) || (cig->hdr.type == UPDATE)) {
        return 1;
    }
    return -1;
}

/* Returns 1 if the gateway seen this device before?
 */
int is_registered(int id, int *device_record) {
    if (device_record[id - LOWEST_ID] == 1) {
        return 1;
    }
    return -1;
}

/* Add a new device to the device_record.  Return the new device id.
 * Note that device ids will never be "de-registered" so they cannot be reused.
 */
int register_device(int *device_record) {
    for (int i = 0; i < MAXDEV; i++) {
        if (device_record[i] == 0) {
            device_record[i] = 1;
            return i + LOWEST_ID;
        }
    }
    return -1;
}

/* Turns on or off the cooler or dehumidifier based on the
 * current temperature or humidity.j
 */
void adjust_fan(struct cignal *cig) {
    cig->hdr.type = 3;
    if (cig->hdr.device_type == TEMPERATURE) {
        if (cig->value >= TEMPSET) {
            cig->cooler = ON;
        } else {
            cig->cooler = OFF;
        }
    } else if (cig->hdr.device_type == HUMIDITY) {
        if (cig->value >= HUMSET) {
            cig->dehumid = ON;
        } else {
            cig->dehumid = OFF;
        }
    }
}

int is_valid_device_id(struct cignal *cig) {
    if (LOWEST_ID <= cig->hdr.device_id && cig->hdr.device_id <= (LOWEST_ID + MAXDEV - 1)) {
        return 1;
    } else if (cig->hdr.type == HANDSHAKE && cig->hdr.device_id == -1) {
        return 1;
    }
    return -1;
}

int is_valid_device_type(struct cignal *cig) {
    if ((cig->hdr.device_type == TEMPERATURE) || (cig->hdr.device_type == HUMIDITY)) {
        return 1;
    }
    return -1;
}

int is_valid_hdr(struct cignal *cig) {
    int check = 0;
    check += is_valid_type(cig);
    check += is_valid_device_id(cig);
    check += is_valid_device_type(cig);
    if (check == 3) {
        return 1;
    }
    return -1;
}

int is_valid_sensor(struct cignal *cig) {
    int be_checked = cig->value;
    int count = 0;
    while (be_checked != 0) {
        be_checked /= 10;
        count ++;
    }
    if (count < 2 || count == 2) {
        return 1;
    }
    return -1;
}

int is_valid_ac_config(struct cignal *cig) {
    if (cig->cooler == ON || cig->cooler == OFF) {
        if (cig->dehumid == ON || cig->dehumid == OFF) {
            return 1;
        }
    }
    return -1;
}

/* Check each field of the incoming header to ensure that it is valid, print
 * information process about the state of the sensor using the printf messages
 * below, and adjust the fan.
 * 
 * Error messages must be printed to stderr, but the contents of the messages
 * are unspecified so you can choose good messages.
 * 
 * Print the following before returning from handling a valid event.
 * printf("********************END EVENT********************\n\n");
 * 
 * Print the following after getting a value from the relevant sensor.
 * printf("Temperature: %.4f --> Device_ID: %d\n", YOUR VARIABLES HERE);
 * printf("Humidity: %.4f --> Device_ID: %d\n", YOUR VARIABLES HERE);
 */

int process_message(struct cignal *cig, int *device_record) {
    
    // TODO
    int to_check = 0;
    to_check += is_valid_hdr(cig);
    to_check += is_valid_sensor(cig);
    to_check += is_valid_ac_config(cig);
    if (to_check == 3) {
        if (cig->hdr.device_type == TEMPERATURE) {
            printf("Temperature: %.4f --> Device_ID: %d\n", cig->value, cig->hdr.device_id);
        } else if (cig->hdr.device_type == HUMIDITY) {
            printf("Humidity: %.4f --> Device_ID: %d\n", cig->value, cig->hdr.device_id);
        }
        switch (cig->hdr.type) {
            // if it is HANDSHAKE message
            case HANDSHAKE:
                //register the device
                cig->hdr.device_id = register_device(device_record);
                break;
            case UPDATE:
                //check if it is registered
                if (is_registered(cig->hdr.device_id, device_record)) {
                    //manage the sent-back message
                    adjust_fan(cig);
                    break;
                }
                fprintf(stderr, "Sensor is not registered yet!\n");
                return -1;
        }
        printf("********************END EVENT********************\n\n");
        return 1;
    }
    
    fprintf(stderr, "Received corrupted cignal! The message is discarded...\n");
    return -1;
}
