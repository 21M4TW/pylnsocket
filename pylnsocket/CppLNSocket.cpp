#include <stdio.h>
#include <assert.h>

#include <sys/select.h>

#include "CppLNSocket.h"

extern "C" {
#include "typedefs.h"
#include "commando.h"
#include "lnsocket.h"
};

int CppLNSocket::Init(const char* nodeid, const char* host)
{
	verbose = getenv("VERBOSE") != 0;
	int ok = 1;
	FD_ZERO(&set); /* clear the set */
	ln = lnsocket_create();
	assert(ln);
	lnsocket_genkey(ln);

	const char* timeout_str = getenv("LNRPC_TIMEOUT");
	int timeout_ms = timeout_str ? atoi(timeout_str) : 50000000;

	timeout.tv_sec = timeout_ms / 1000;
	timeout.tv_usec = (timeout_ms % 1000) * 1000;

	if (!(ok = lnsocket_connect(ln, nodeid, host)))
		goto done_init;

	if (!(ok = lnsocket_fd(ln, &socket)))
		goto done_init;

	FD_SET(socket, &set); /* add our file descriptor to the set */

	if (!(ok = lnsocket_perform_init(ln)))
		goto done_init;

	if (verbose)
		fprintf(stderr, "init success\n");

done_init:
	lnsocket_print_errors(ln);
	lnsocket_destroy(ln);
	return !ok;
}

int CppLNSocket::Call(const char* method, const char* params, const char* rune, std::string* ret)
{
	static u8 msgbuf[4096];
	u8 *buf;
	int ok, rv;
	u16 len, msgtype;
	const unsigned int req_id = 1;

	ret->clear();

	if (!(ok = len = commando_make_rpc_msg(method, params, rune, req_id, msgbuf, sizeof(msgbuf))))
		goto done_call;

	if (!(ok = lnsocket_write(ln, msgbuf, len)))
		goto done_call;

	if (verbose)
		fprintf(stderr, "waiting for response...\n");

	while (1) {
		rv = select(socket + 1, &set, NULL, NULL, &timeout);

		if (rv == -1) {
			perror("select");
			ok = 0;
			goto done_call;
		} else if (rv == 0) {
			fprintf(stderr, "error: rpc request timeout\n");
			ok = 0;
			goto done_call;
		}

		if (!(ok = lnsocket_recv(ln, &msgtype, &buf, &len)))
			goto done_call;

		switch (msgtype) {
			case COMMANDO_REPLY_TERM:
				//printf("%.*s\n", len - 8, buf + 8);
				ret->append((const char*)buf + 8, len - 8);
				goto done_call;
			case COMMANDO_REPLY_CONTINUES:
				//printf("%.*s", len - 8, buf + 8);
				ret->append((const char*)buf + 8, len - 8);
				continue;
			case WIRE_PING:
				if (!lnsocket_pong(ln, buf, len)) {
					fprintf(stderr, "pong error...\n");
				}
			default:
				// ignore extra interleaved messages which can happen
				continue;
		}
	}

done_call:
	lnsocket_print_errors(ln);
	lnsocket_destroy(ln);
	return !ok;
}
