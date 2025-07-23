#include <string>

extern "C" {
#include "lnsocket.h"  // Header from the static lib
};

class CppLNSocket {
	public:
		int Init(const char* nodeid, const char* host);
		int Call(const char* method, const char* params, const char* rune, std::string* ret);
	private:
		struct lnsocket *ln;
		int socket;
		fd_set set;
	        struct timeval timeout;
		bool verbose;
};
