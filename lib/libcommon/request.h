/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <stdint.h>

class request_t
{
	struct handler_t
	{
		handler_t *next;
		void (*handler)(uint8_t param, uint16_t data);
	} *handlers;
	
public:
	request_t() : handlers(0), requests(0) {}
	void registerISR(void (*handler)(uint8_t, uint16_t));
	void request(uint8_t param);
	void isr();

protected:
	struct req_t
	{
		req_t *next;
		uint8_t param;
	} *requests;

	virtual void execute(req_t *req) = 0;
	virtual bool available() = 0;
	virtual uint16_t getResult() = 0;
	virtual uint8_t getParam() = 0;

private:
	req_t *dequeue();
	void next();
};

#endif
