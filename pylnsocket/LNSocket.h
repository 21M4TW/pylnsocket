extern "C" {
#include "lnsocket.h"  // Header from the static lib
};

class LNSocket {
	public:
		int init(const char* nodeid, const char* host);
	private:
		struct lnsocket *ln;
		int socket;
		fd_set set;
};
