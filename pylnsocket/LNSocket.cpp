#include <stdio.h>
#include <assert.h>

#include <sys/select.h>

#include "LNSocket.h"

int LNSocket::init(const char* nodeid, const char* host)
{
	int verbose = getenv("VERBOSE") != 0;
	int ok = 1;
	FD_ZERO(&set); /* clear the set */
	ln = lnsocket_create();
	assert(ln);
	lnsocket_genkey(ln);

	if (!(ok = lnsocket_connect(ln, nodeid, host)))
		goto done;

	if (!(ok = lnsocket_fd(ln, &socket)))
		goto done;

	FD_SET(socket, &set); /* add our file descriptor to the set */

	if (!(ok = lnsocket_perform_init(ln)))
		goto done;

	if (verbose)
		fprintf(stderr, "init success\n");

done:
	lnsocket_print_errors(ln);
	lnsocket_destroy(ln);
	return !ok;
}
