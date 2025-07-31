from pylnsocket cimport LNSocket
from libc.string cimport memcpy
from cpython.ref cimport PyObject
from cpython.memoryview cimport PyMemoryView_FromMemory
from cpython.buffer cimport PyBUF_READ
import json
import ijson


class RpcError(ValueError):
    def __init__(self, method: str, payload: dict, error):

        self.method = method
        self.payload = payload

        if isinstance(error, str):
            self.error = error
        elif isinstance(error, dict):

            if error.get('code') and isinstance(error['code'], int) and error.get('message') and isinstance(error['message'], str):
                self.error = {'code': error['code'], 'message': error['message']}
            else:
                self.error = "Unknown error format"
        else:
            self.error = "Unknown error format"

        super(ValueError, self).__init__(
            "RPC call failed: method: {}, payload: {}, error: {}".format(
                self.method, self.payload, self.error
            )
        )


cdef class LNSocket:
    cdef CppLNSocket* _impl

    def __cinit__(self, nodeid: str, host: str):
        bnodeid = nodeid.encode('ASCII')
        cdef const char* cnodeid = bnodeid
        bhost = host.encode('ASCII')
        cdef const char* chost = bhost
        self._impl = new CppLNSocket(cnodeid, chost)

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

    def Call(self, rune: bytes, method: str, params = None) -> Call:
        bmethod = method.encode('ASCII')
        cdef const char* crune = rune
        cdef const char* cmethod = bmethod
        cdef char* ret
        cdef uint16_t retlen
        cdef const char* cparams
        cdef bool loop

        resp, receiver_coro = self._collector()
        coro = ijson.kvitems_coro(receiver_coro, '')

        if params:

            if isinstance(params, str):
                bparams = params.encode('ASCII')

            else:
                bparams = json.dumps(params).encode('ASCII')
            cparams = bparams
            loop = self._impl.Call(&ret, &retlen, crune, cmethod, cparams)
        else:
            loop = self._impl.Call(&ret, &retlen, crune, cmethod)
        coro.send(PyMemoryView_FromMemory(ret, retlen, PyBUF_READ))

        while loop:
            loop = self._impl.Call(&ret, &retlen)
            coro.send(PyMemoryView_FromMemory(ret, retlen, PyBUF_READ))
        coro.close()
        
        if not isinstance(resp, dict):
            raise TypeError("Malformed response, response is not a dictionary %s." % resp)
        elif "error" in resp:
            raise RpcError(method, params, resp['error'])
        elif "result" not in resp:
            raise ValueError("Malformed response, \"result\" missing.")
        return resp["result"]
