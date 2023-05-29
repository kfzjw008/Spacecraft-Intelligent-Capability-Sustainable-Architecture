//
// Created by kfzjw008 on 2023-05-16.
//

#ifndef SICSA_DATAINJECTION_H
#define SICSA_DATAINJECTION_H
#define INJECTION_PACKET_TYPE 1
#define NO_SECONDARY_HEADER 0
#define APPLICATION_PROCESS_ID_WITHOUT_SECONDARY_HEADER 0x1334
#define APPLICATION_PROCESS_ID_WITH_SECONDARY_HEADER 0x1B34
#include <stdint.h>

void injectionPacketReception(uint8_t data[], uint16_t size);
uint16_t packetIdentification(uint8_t *data, uint16_t size);


#endif //SICSA_DATAINJECTION_H

