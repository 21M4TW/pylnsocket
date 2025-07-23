from pylnsocket cimport LNSocket
from libcpp.string cimport string

cdef class LNSocket:
    cdef CppLNSocket* _impl

    def __cinit__(self):
        self._impl = new CppLNSocket()

    def __dealloc__(self):
        del self._impl

    def Init(self, nodeid: str, host: str) -> Init:
        bnodeid = nodeid.encode('utf-8')
        cdef const char* cnodeid = bnodeid
        bhost = host.encode('utf-8')
        cdef const char* chost = bhost
        return self._impl.Init(cnodeid, chost)

    def Call(self, method: str, params: str, rune: str) -> Call:
        bmethod = method.encode('utf-8')
        cdef const char* cmethod = bmethod
        bparams = params.encode('utf-8')
        cdef const char* cparams = bparams
        brune = rune.encode('utf-8')
        cdef const char* crune = brune
        cdef string ret
        ok = self._impl.Call(cmethod, cparams, crune, &ret)
        uret = ret.decode('UTF-8')
        return ok, uret
