from os import environ

initialized: bool = False

if not initialized:

    from ctypes import Structure, c_uint32
    from ctypes import (
        cdll,
        CDLL,
        c_void_p,
        c_char_p,
        c_uint,
        c_bool,
        c_int,
        c_long,
        c_size_t,
        POINTER,
    )
    
    class PLUTO_EDGE_Timestamp(Structure):
        _fields_ = [
            ("year", c_uint32),
            ("month", c_uint32),
            ("day", c_uint32),
            ("hour", c_uint32),
            ("minutes", c_uint32),
            ("seconds", c_uint32),
            ("milliseconds", c_uint32),
        ]
        pass
    
    shared_lib_extension: str = environ.get('PLUT_SHARED_LIBRARY_EXTENSION', default='dylib')
    library_path: str = f"/usr/local/lib/libpluto_edge_shared.{shared_lib_extension}"
    _lib_pluto_edge: CDLL = cdll.LoadLibrary(
        library_path,
    )
    if _lib_pluto_edge is None:
        raise AssertionError(f'Unable to load shared Library from "{library_path}"')

    # Logger.
    _lib_pluto_edge.PLUTO_CreateLogger.argtypes = [
        c_char_p,
    ]
    _lib_pluto_edge.PLUTO_CreateLogger.restype = c_void_p

    # Event.
    _lib_pluto_edge.PLUTO_EDGE_CreateEvent.argtypes = []
    _lib_pluto_edge.PLUTO_EDGE_CreateEvent.restype = c_void_p

    _lib_pluto_edge.PLUTO_EDGE_DestroyEvent.argtypes = [
        POINTER(c_void_p),
    ]

    _lib_pluto_edge.PLUTO_EDGE_EventSetId.argtypes = [
        c_void_p,
        c_uint,
    ]
    _lib_pluto_edge.PLUTO_EventSetId.restype = c_void_p

    _lib_pluto_edge.PLUTO_EDGE_EventSetEventId.argtypes = [
        c_void_p,
        c_uint,
    ]
    _lib_pluto_edge.PLUTO_EDGE_EventSetEventId.restype = c_void_p

    # Edge.

    _lib_pluto_edge.PLUTO_EDGE_CreateEdge.argtypes = [
        c_char_p,
        c_char_p,
        c_uint,
        c_void_p,
    ]
    _lib_pluto_edge.PLUTO_EDGE_CreateEdge.restype = c_void_p

    _lib_pluto_edge.PLUTO_EDGE_DestroyEdge.argtypes = [
        POINTER(c_void_p),
    ]

    _lib_pluto_edge.PLUTO_EDGE_EdgeSendEvent.argtypes = [
        c_void_p,
        c_void_p,
    ]
    _lib_pluto_edge.PLUTO_EDGE_EdgeSendEvent.restype = c_bool

    _lib_pluto_edge.PLUTO_EDGE_EdgeReceiveEvent.argtypes = [
        c_void_p,
        c_void_p,
    ]
    _lib_pluto_edge.PLUTO_EDGE_EdgeReceiveEvent.restype = c_bool

    initialized = True
