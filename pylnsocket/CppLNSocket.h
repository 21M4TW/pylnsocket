#include <string>

extern "C" {
#include "lnsocket.h"  // Header from the static lib
};

class CppLNSocket {
	public:
		CppLNSocket(): ln(NULL){}
		virtual ~CppLNSocket(){DeInit();}
		void Init(const char* nodeid, const char* host);
		void DeInit();
		bool Call(char** const ret, uint16_t* retlen, const char* rune=NULL, const char* method=NULL, const char* params=NULL);
	private:
		struct lnsocket *ln;
		int socket;
		fd_set set;
	        struct timeval timeout;
		bool verbose;
};
