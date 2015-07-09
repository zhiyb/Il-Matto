#ifndef EVENT_H
#define EVENT_H

namespace event
{
	enum Status {None = 0, WlanConnection, DHCP, SmartConfigFinished};
	union Result {
		uint32_t ip;
	};

	// Initialise event variables
	void init(void);
	bool dhcpFinished(void);
	bool wlanConnected(void);
	const uint32_t ipAddress(void);
	bool smartConfigFinished(void);
	Status pool(void);
	// The function handles asynchronous events that come from CC3000
	void usynchCallback(long lEventType, char *data, unsigned char length);
}

#endif
