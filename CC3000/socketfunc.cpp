#include <wlan.h>
#include <hci.h>
#include <socket.h>
#include <string.h>
#include "menu.h"
#include "event.h"

static int32_t skt = -1;

bool menu::socket::socket::func(bool enter)
{
	if (skt != -1)
		closesocket::func(enter);
	else
		tftClean();
	if (skt != -1)
		return true;
	puts_P(PSTR("Creating socket..."));
	skt = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (skt == -1)
		puts_P(PSTR("Error."));
	else
		printf_P(PSTR("Socket: %ld\n"), skt);
	touch.waitForPress();
	return true;
}

bool menu::socket::bind::func(bool enter)
{
	if (skt == -1)
		socket::func(enter);
	else
		tftClean();
	if (skt == -1)
		return true;
	puts_P(PSTR("Binding to 43690..."));
	sockaddr addr;
	addr.sa_family = AF_INET;
	addr.sa_data[0] = 0xAA;
	addr.sa_data[1] = 0xAA;
	memset(&addr.sa_data[2], 0, 4);
	int32_t res = ::bind(skt, &addr, sizeof(sockaddr));
	if (res == 0)
		puts_P(PSTR("Binding success."));
	else
		printf_P(PSTR("Error: %ld\n"), res);
	touch.waitForPress();
	return true;
}

bool menu::socket::connect::func(bool enter)
{
	if (skt == -1)
		socket::func(enter);
	else
		tftClean();
	if (skt == -1)
		return true;
	puts_P(PSTR("Connecting to:\n192.168.0.12:6666"));

	sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(6666);
	const uint8_t ip[4] = {192, 168, 0, 12};
	destAddr.sin_addr.s_addr = *(uint32_t *)ip;
	memset(destAddr.sin_zero, 0, 8);

	int32_t res = ::connect(skt, (sockaddr *)&destAddr, sizeof(sockaddr));
	if (res == 0)
		puts_P(PSTR("Connection success."));
	else
		printf_P(PSTR("Error: %ld\n"), res);
	touch.waitForPress();
	return true;
}

bool menu::socket::send::func(bool enter)
{
	if (skt == -1)
		connect::func(enter);
	else
		tftClean();
	if (skt == -1)
		return true;
	puts_P(PSTR("Sending HW..."));
	char str[] = "Hello, world!\n";
	int32_t res = ::send(skt, str, strlen(str) + 1, 0);
	if (res == 0)
		puts_P(PSTR("Sent."));
	else
		printf_P(PSTR("Error: %ld\n"), res);
	touch.waitForPress();
	return true;
}

bool menu::socket::closesocket::func(bool enter)
{
	tftClean();
	if (skt == -1) {
		puts_P(PSTR("Socket closed."));
		touch.waitForPress();
		return true;
	}
	puts_P(PSTR("Closing socket..."));
	int32_t res = ::closesocket(skt);
	if (res == 0) {
		puts_P(PSTR("Socket closed."));
		skt = -1;
	} else
		printf_P(PSTR("Error: %ld\n"), res);
	touch.waitForPress();
	return true;
}

bool menu::socket::newsocket::func(bool enter)
{
	skt = -1;
	return socket::func(enter);
}
