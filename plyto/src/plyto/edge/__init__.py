
initialized: bool = False

if not initialized:

    from ctypes import cdll, CDLL, c_void_p, c_char_p, c_uint, c_bool, c_int, c_long, c_size_t, POINTER

    library_path: str = '/usr/local/lib/libpluto_edge_shared.dylib'
    _lib_pluto_edge: CDLL = cdll.LoadLibrary(
        library_path,
    )
    if _lib_pluto_edge is None:
        raise AssertionError(
            f'Unable to load shared Library from "{library_path}"'
        )
    
    # Logger.
    _lib_pluto_edge.PLUTO_CreateLogger.argtypes = [
        c_char_p,
    ]
    _lib_pluto_edge.PLUTO_CreateLogger.restype = c_void_p


    # Event.
    _lib_pluto_edge.PLUTO_CreateEvent.argtypes = []
    _lib_pluto_edge.PLUTO_CreateEvent.restype = c_void_p

    _lib_pluto_edge.PLUTO_DestroyEvent.argtypes = [
        POINTER(c_void_p),
    ]
    
    _lib_pluto_edge.PLUTO_EventSetId.argtypes = [
        c_void_p, c_uint,
    ]
    _lib_pluto_edge.PLUTO_EventSetId.restype = c_void_p

    _lib_pluto_edge.PLUTO_EventSetEvent.argtypes = [
        c_void_p, c_uint,
    ]
    _lib_pluto_edge.PLUTO_EventSetEvent.restype = c_void_p
    

    from ctypes import Structure, c_uint32
    class tm(Structure):
        _fields_ = [
            ('tm_sec', c_int),
            ('tm_min', c_int),
            ('tm_hour', c_int),
            ('tm_mday', c_int),
            ('tm_mon', c_int),
            ('tm_year', c_int),
            ('tm_wday', c_int),
            ('tm_yday', c_int),
            ('tm_isdst', c_int),
            ('tm_gmtoff', c_long),
            ('tm_zone', c_char_p),
        ]
        pass

    class PlutoTimestamp(Structure):
        _fields_ = [
            ('tm', tm),
            ('milliseconds', c_uint32),
        ]
        pass

    # Os Abstraction
    
    _lib_pluto_edge.PLUTO_TimeNow.argtypes = []
    _lib_pluto_edge.PLUTO_TimeNow.restype = PlutoTimestamp

    #Event.

    _lib_pluto_edge.PLUTO_EventSetTimestamp.argtypes = [
        c_void_p, PlutoTimestamp,
    ]
    _lib_pluto_edge.PLUTO_EventSetTimestamp.restype = c_void_p

    _lib_pluto_edge.PLUTO_EventPayload.argtypes = [
        c_void_p,
    ]
    _lib_pluto_edge.PLUTO_EventPayload.restype = c_char_p
    
    _lib_pluto_edge.PLUTO_EventCopyBufferToPayload.argtypes = [
        c_void_p, c_void_p, c_size_t, 
    ]
    _lib_pluto_edge.PLUTO_EventCopyBufferToPayload.restype = c_bool

    _lib_pluto_edge.PLUTO_EventSetSizeOfPayload.argtypes = [
        c_void_p, c_size_t, 
    ]

    _lib_pluto_edge.PLUTO_EventSizeOfPayload.argtypes = [
        c_void_p,
    ]
    _lib_pluto_edge.restype = c_size_t

    _lib_pluto_edge.PLUTO_EventToBuffer.argtypes = [
        c_void_p, c_char_p, c_size_t,
    ]
    _lib_pluto_edge.PLUTO_EventToBuffer.restype = c_bool

    # Edge.

    _lib_pluto_edge.PLUTO_EDGE_CreateEdge.argtypes = [
        c_char_p, c_char_p, c_uint, c_void_p,
    ]
    _lib_pluto_edge.PLUTO_EDGE_CreateEdge.restype = c_void_p

    _lib_pluto_edge.PLUTO_EDGE_DestroyEdge.argtypes = [
        POINTER(c_void_p),
    ]

    _lib_pluto_edge.PLUTO_EDGE_EdgeSendEvent.argtypes = [
        c_void_p, c_void_p, 
    ]
    _lib_pluto_edge.PLUTO_EDGE_EdgeSendEvent.restype = c_bool 

    _lib_pluto_edge.PLUTO_EDGE_EdgeReceiveEvent.argtypes = [
        c_void_p, c_void_p,
    ]
    _lib_pluto_edge.PLUTO_EDGE_EdgeReceiveEvent.restype = c_bool 

    initialized = True


