/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <stdlib.h>
#include "request.h"

request_t::req_t *request_t::dequeue(void)
{
	req_t *s = requests;
	if (s)
		requests = s->next;
	return s;
}

void request_t::registerISR(void (*handler)(uint8_t, uint16_t))
{
	handler_t *s = (request_t::handler_t *)malloc(sizeof(handler_t));
	s->next = handlers;
	s->handler = handler;
	handlers = s;
}

void request_t::next(void)
{
	req_t *req = dequeue();
	if (req) {
		execute(req);
		free(req);
	}
}

void request_t::request(uint8_t param)
{
	req_t *req = (request_t::req_t *)malloc(sizeof(req_t));
	req->next = 0;
	req->param = param;

	if (!requests && available()) {
		execute(req);
		free(req);
		return;
	}

	req_t **p = &requests;
	while (*p)
		p = &(*p)->next;
	*p = req;
	if (available())
		next();
}

void request_t::isr(void)
{
	uint16_t result = getResult();
	uint8_t ch = getParam();
	next();
	struct handler_t *p = handlers;
	while (p) {
		p->handler(ch, result);
		p = p->next;
	}
}
