"""
Exception classes for the Plyto client library.
"""
# ----------------------------------------------------------------------------------------------------------------------


class PlytoException(Exception):
    pass

class PlytoNoEventAvailableExcpetion(PlytoException):
    pass

class PlytoPayloadToLargeException(PlytoException):
    pass

class PlytoTimestampException(PlytoException):
    pass

class PlytoAddressNotSetException(PlytoException):
    pass

class PlytoRequestFailedException(PlytoException):
    pass

# ----------------------------------------------------------------------------------------------------------------------
