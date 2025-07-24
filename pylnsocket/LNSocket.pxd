from libcpp.string cimport string

cdef extern from "CppLNSocket.h":
    cdef cppclass CppLNSocket:
        CppLNSocket()
        void Init(const char* nodeid, const char* host, const char* rune) except +
        void Call(string* ret, const char* method, const char* params) except +
        void Call(string* ret, const char* method) except +
