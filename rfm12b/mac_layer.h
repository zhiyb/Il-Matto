#ifndef MAC_LAYER
#define MAC_LAYER

#ifdef __cplusplus
extern "C" {
#endif

// Probability of 0.1
#define MAC_CSMA	(25 + 2)

void mac_csma_0_tick();
void mac_csma_1_tick();
void mac_csma_p_tick(uint8_t p);

static inline void mac_tick(uint8_t csma)
{
	switch (csma) {
	case 0:
		mac_csma_0_tick();
		break;
	case 1:
		mac_csma_1_tick();
		break;
	case 2:
	default:
		mac_csma_p_tick(csma - 2);
	}
}

#ifdef __cplusplus
}
#endif

#endif
