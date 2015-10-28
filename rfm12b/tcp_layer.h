#ifndef TCP_H
#define TCP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MACHINE_ADDRESS	((uint8_t)(0x55))

uint8_t *payloadBuffer();
uint8_t send(uint8_t addr, uint8_t length);
uint8_t write(uint8_t addr, uint8_t length, uint8_t *buffer);
uint8_t available();
uint8_t read(uint8_t *addr, uint8_t *buffer);

#ifdef __cplusplus
}
#endif

#endif
