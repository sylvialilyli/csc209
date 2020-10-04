//
// Created by lsyli on 12/5/2019.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"

char *serialize_cignal(struct cignal cig){
    char *result = malloc(sizeof(char)*CIGLEN);
    snprintf(result, CIGLEN, "%d|%d|%d|%.4f|%d|%d|",
             cig.hdr.device_id,
             cig.hdr.device_type,
             cig.hdr.type,
             cig.value,
             cig.cooler,
             cig.dehumid);

    return result;
}

void unpack_cignal(char *msg, struct cignal *cig){
    sscanf(msg, "%d|%d|%d|%f|%d|%d|",
           &cig->hdr.device_id,
           &cig->hdr.device_type,
           &cig->hdr.type,
           &cig->value,
           &cig->cooler,
           &cig->dehumid);
}

int main() {
    struct cignal there;
    there.hdr.device_id = 80;
    there.hdr.type = 2;
    there.hdr.device_type = 1;
    there.cooler = ON;
    there.dehumid = OFF;
    there.value = 12.2345;
    char *hi = serialize_cignal(there);
    printf("%lu", strlen(hi));

    free(hi);

    return 0;
}