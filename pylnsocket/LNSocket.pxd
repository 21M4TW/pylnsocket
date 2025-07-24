from libcpp.string cimport string

cdef extern from "CppLNSocket.h":
    cdef cppclass CppLNSocket:
        CppLNSocket()
        void Init(const char* nodeid, const char* host) except +
        void Call(const char* method, const char* params, const char* rune, string* ret) except +
