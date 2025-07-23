cdef extern from "CppLNSocket.h":
    cdef cppclass CppLNSocket:
        CppLNSocket()
        int init(const char* nodeid, const char* host)
