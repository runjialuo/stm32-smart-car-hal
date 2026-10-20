#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>
void Delay_Init(void);

void Delay_us(uint32_t xus);

void Delay_ms(uint32_t xms);

void Delay_s(uint32_t xs);

#endif
