// Author: Yubo Zhi (normanzyb@gmail.com)

#ifndef SAMPLER_H
#define SAMPLER_H

#ifdef __cplusplus
extern "C" {
#endif

void sampler_init();
void sampler_enable(const uint8_t e);
uint8_t sampler_available();
uint8_t sampler_get();
uint8_t sampler_tick();

#ifdef __cplusplus
}
#endif

#endif
