cdef extern from "LNSocket.h":
    cdef cppclass LNSocket:
        LNSocket()
        int init(const char* nodeid, const char* host)
