from pylnsocket cimport LNSocket
from libcpp.string cimport string

cdef class LNSocket:
    cdef CppLNSocket* _impl

    def __cinit__(self):
        self._impl = new CppLNSocket()

    def __dealloc__(self):
        del self._impl

    def Init(self, nodeid: str, host: str) -> Init:
        bnodeid = nodeid.encode('UTF-8')
        cdef const char* cnodeid = bnodeid
        bhost = host.encode('UTF-8')
        cdef const char* chost = bhost
        self._impl.Init(cnodeid, chost)

    def Call(self, method: str, params: str, rune: str) -> Call:
        bmethod = method.encode('UTF-8')
        cdef const char* cmethod = bmethod
        bparams = params.encode('UTF-8')
        cdef const char* cparams = bparams
        brune = rune.encode('UTF-8')
        cdef const char* crune = brune
        cdef string ret
        self._impl.Call(cmethod, cparams, crune, &ret)
        uret = ret.decode('UTF-8')
        return uret
