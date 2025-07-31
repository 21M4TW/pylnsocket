#include <string>

extern "C" {
#include "lnsocket.h"  // Header from the static lib
};

class CppLNSocket {
	public:
		CppLNSocket(const char* nodeid, const char* host);
		virtual ~CppLNSocket();
		bool Call(char** const ret, uint16_t* retlen, const char* rune=NULL, const char* method=NULL, const char* params=NULL);
	private:
		CppLNSocket(){}
		CppLNSocket(const CppLNSocket&){}
		const CppLNSocket& operator=(const CppLNSocket&){return *this;}
		struct lnsocket *ln;
		int socket;
		fd_set set;
	        struct timeval timeout;
		bool verbose;
};
