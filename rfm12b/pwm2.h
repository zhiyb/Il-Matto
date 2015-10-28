#ifndef PWM2_H
#define PWM2_H

#ifdef __cplusplus
extern "C" {
#endif

void pwm2_init();
void pwm2_enable(uint8_t e);
void pwm2_set(const uint8_t value);
uint8_t pwm2_get();

#ifdef __cplusplus
}
#endif

#endif
