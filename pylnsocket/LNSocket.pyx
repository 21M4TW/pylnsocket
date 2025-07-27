from pylnsocket cimport LNSocket
from libc.string cimport memcpy
from cpython.ref cimport PyObject
from cpython.memoryview cimport PyMemoryView_FromMemory
from cpython.buffer cimport PyBUF_READ
import ijson


cdef class LNSocket:
    cdef CppLNSocket* _impl

    def __cinit__(self):
        self._impl = new CppLNSocket()

    def __dealloc__(self):
        del self._impl

    @staticmethod
    def _collector():
        results = {}
        def _receiver():
            while True:
                obj = (yield)
                results[obj[0]] = obj[1]
        return results, ijson.coroutine(_receiver)()

    def Init(self, nodeid: str, host: str) -> Init:
        bnodeid = nodeid.encode('ASCII')
        cdef const char* cnodeid = bnodeid
        bhost = host.encode('ASCII')
        cdef const char* chost = bhost
        self._impl.Init(cnodeid, chost)

    def Call(self, rune: bytes, method: str, params = None) -> Call:
        bmethod = method.encode('ASCII')
        cdef const char* crune = rune
        cdef const char* cmethod = bmethod
        cdef char* ret
        cdef uint16_t retlen
        cdef const char* cparams
        cdef bool loop

        results, receiver_coro = self._collector()
        coro = ijson.kvitems_coro(receiver_coro, '')

        if params:
            bparams = str(params).encode('ASCII')
            cparams = bparams
            loop = self._impl.Call(&ret, &retlen, crune, cmethod, cparams)
        else:
            loop = self._impl.Call(&ret, &retlen, crune, cmethod)
        coro.send(PyMemoryView_FromMemory(ret, retlen, PyBUF_READ))

        while loop:
            loop = self._impl.Call(&ret, &retlen)
            coro.send(PyMemoryView_FromMemory(ret, retlen, PyBUF_READ))
        coro.close()
        return results
