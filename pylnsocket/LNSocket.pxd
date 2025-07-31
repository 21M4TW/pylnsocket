cdef extern from "stdint.h":
        ctypedef unsigned short uint16_t
cdef extern from "stdbool.h":
        ctypedef bint bool

cdef extern from "CppLNSocket.h":
    cdef cppclass CppLNSocket:
        CppLNSocket(const char* nodeid, const char* host) except +
        bool Call(char** ret, uint16_t* retlen, const char* rune, const char* method, const char* params) except +
        bool Call(char** ret, uint16_t* retlen, const char* rune, const char* method) except +
        bool Call(char** ret, uint16_t* retlen) except +
