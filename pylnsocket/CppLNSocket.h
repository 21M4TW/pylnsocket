#include <string>

extern "C" {
#include "lnsocket.h"  // Header from the static lib
};

class CppLNSocket {
	public:
		CppLNSocket(): ln(NULL){}
		virtual ~CppLNSocket(){DeInit();}
		void Init(const char* nodeid, const char* host, const char* rune);
		void DeInit();
		void Call(std::string* ret, const char* method, const char* params=NULL);
	private:
		struct lnsocket *ln;
		char* rune;
		size_t runeLength;
		int socket;
		fd_set set;
	        struct timeval timeout;
		bool verbose;
};
