"""
Exception classes for the Plyto client library.
"""

# ----------------------------------------------------------------------------------------------------------------------


class PlytoException(Exception):
    """Base exception for all Plyto errors."""


class PlytoNoEventAvailableExcpetion(PlytoException):
    """Raised when no event is available to receive."""


class PlytoPayloadToLargeException(PlytoException):
    """Raised when an event payload exceeds the allowed size."""


class PlytoTimestampException(PlytoException):
    """Raised when an event timestamp is invalid or missing timezone information."""


class PlytoAddressNotSetException(PlytoException):
    """Raised when no server address is configured."""


class PlytoRequestFailedException(PlytoException):
    """Raised when an HTTP request to the Pluto server fails."""


# ----------------------------------------------------------------------------------------------------------------------
