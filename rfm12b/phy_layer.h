#ifndef PHY_LAYER
#define PHY_LAYER

#ifdef __cplusplus
extern "C" {
#endif

#include <rfm12_config.h>
#include <rfm12.h>

static inline void RFM12_setBaudrate(uint16_t baud) {rfm12_livectrl(RFM12_LIVECTRL_DATARATE, baud);}
static inline void RFM12_setCenterFrequency(uint16_t freq) {rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq);}

#ifdef __cplusplus
}
#endif

#endif
