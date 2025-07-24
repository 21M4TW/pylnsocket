from pylnsocket cimport LNSocket
from libcpp.string cimport string

cdef class LNSocket:
    cdef CppLNSocket* _impl

    def __cinit__(self):
        self._impl = new CppLNSocket()

    def __dealloc__(self):
        del self._impl

    def Init(self, nodeid: str, host: str, brune: bytearray) -> Init:
        bnodeid = nodeid.encode('UTF-8')
        cdef const char* cnodeid = bnodeid
        bhost = host.encode('UTF-8')
        cdef const char* chost = bhost
        cdef const char* crune = brune
        self._impl.Init(cnodeid, chost, crune)
        brune[:] = map(lambda x: 0, brune)
        del brune

    def Call(self, method: str, params: str = None) -> Call:
        bmethod = method.encode('UTF-8')
        cdef const char* cmethod = bmethod
        cdef string ret
        cdef const char* cparams

        if params:
            bparams = params.encode('UTF-8')
            cparams = bparams
            self._impl.Call(&ret, cmethod, cparams)
        else:
            self._impl.Call(&ret, cmethod)
        uret = ret.decode('UTF-8')
        return uret
