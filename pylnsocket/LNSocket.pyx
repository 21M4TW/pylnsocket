from pylnsocket cimport LNSocket
from libcpp.string cimport string

cdef class LNSocket:
    cdef CppLNSocket* _impl

    def __cinit__(self):
        self._impl = new CppLNSocket()

    def __dealloc__(self):
        del self._impl

    def init(self, nodeid: str, host: str) -> init:
        bnodeid = nodeid.encode('utf-8')
        cdef const char* cnodeid = bnodeid
        bhost = host.encode('utf-8')
        cdef const char* chost = bhost
        return self._impl.init(cnodeid, chost)
