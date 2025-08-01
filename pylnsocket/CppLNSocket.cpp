#include <cstdio>
#include <cstring>
#include <strings.h>

#include <stdexcept>

#include <sys/select.h>

#include "CppLNSocket.h"

extern "C" {
#include "typedefs.h"
#include "commando.h"
#include "lnsocket.h"
};

using namespace std;

CppLNSocket::CppLNSocket(const char* nodeid, const char* host)
{
	verbose = getenv("VERBOSE") != 0;
	ln = lnsocket_create();
	
	if(!ln) throw runtime_error("lnsocket_create failed");

	FD_ZERO(&set); /* clear the set */
	lnsocket_genkey(ln);

	const char* timeout_str = getenv("LNRPC_TIMEOUT");
	int timeout_ms = timeout_str ? atoi(timeout_str) : 50000000;

	timeout.tv_sec = timeout_ms / 1000;
	timeout.tv_usec = (timeout_ms % 1000) * 1000;

	if (!lnsocket_connect(ln, nodeid, host)) {
		lnsocket_print_errors(ln);
		throw runtime_error("lnsocket_connect failed");
	}

	if (!lnsocket_fd(ln, &socket)) {
		lnsocket_print_errors(ln);
		throw runtime_error("lnsocket_fd failed");
	}

	FD_SET(socket, &set); /* add our file descriptor to the set */

	if (!lnsocket_perform_init(ln)) {
		lnsocket_print_errors(ln);
		throw runtime_error("lnsocket_perform_init failed");
	}

	if (verbose)
		fprintf(stderr, "init success\n");
}

CppLNSocket::~CppLNSocket()
{
	lnsocket_destroy(ln);
	ln = NULL;
}

bool CppLNSocket::Call(char** const ret, uint16_t* retlen, const char* rune, const char* method, const char* params)
{
	static u8 msgbuf[4096];
	u8 *buf;
	int rv;
	u16 len, msgtype;
	const unsigned int req_id = 1;

	*retlen = 0;

	if(method) {

		if(!rune) rune = "";

		if (!(len = commando_make_rpc_msg(method, params, rune, req_id, msgbuf, sizeof(msgbuf)))) {
			lnsocket_print_errors(ln);
			throw runtime_error("commando_make_rpc_msg failed");;
		}

		if (!lnsocket_write(ln, msgbuf, len)) {
			lnsocket_print_errors(ln);
			throw runtime_error("lnsocket_write failed");;
		}
	}

	if (verbose)
		fprintf(stderr, "waiting for response...\n");

	while (1) {
		rv = select(socket + 1, &set, NULL, NULL, &timeout);

		if (rv == -1) {
			perror("select");
			throw runtime_error("select failed");;
		} else if (rv == 0) {
			fprintf(stderr, "error: rpc request timeout\n");
			throw runtime_error("select timeout");;
		}

		if (!lnsocket_recv(ln, &msgtype, &buf, &len)) {
			lnsocket_print_errors(ln);
			throw runtime_error("lnsocket_recv failed");;
		}

		switch (msgtype) {
			case COMMANDO_REPLY_TERM:
				//printf("%.*s\n", len - 8, buf + 8);
				*ret = (char*)buf + 8;
				*retlen = len - 8;
				return false;
			case COMMANDO_REPLY_CONTINUES:
				//printf("%.*s", len - 8, buf + 8);
				*ret = (char*)buf + 8;
				*retlen = len - 8;
				return true;
			case WIRE_PING:
				if (!lnsocket_pong(ln, buf, len)) {
					fprintf(stderr, "pong error...\n");
				}
				return false;
			default:
				// ignore extra interleaved messages which can happen
				continue;
		}
	}
}
