from libcpp.string cimport string

cdef extern from "CppLNSocket.h":
    cdef cppclass CppLNSocket:
        CppLNSocket()
        int Init(const char* nodeid, const char* host)
        int Call(const char* method, const char* params, const char* rune, string* ret);
