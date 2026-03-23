"""
Exception classes for the Plyto client library.
"""

# ----------------------------------------------------------------------------------------------------------------------


class NewHorizonException(Exception):
    """Base exception for all Plyto errors."""


class NewHorizonNoEventAvailableException(NewHorizonException):
    """Raised when no event is available to receive."""


class NewHorizonPayloadToLargeException(NewHorizonException):
    """Raised when an event payload exceeds the allowed size."""


class NewHorizonTimestampException(NewHorizonException):
    """Raised when an event timestamp is invalid or missing timezone information."""


class NewHorizonAddressNotSetException(NewHorizonException):
    """Raised when no server address is configured."""


class NewHorizonRequestFailedException(NewHorizonException):
    """Raised when an HTTP request to the Pluto server fails."""


# ----------------------------------------------------------------------------------------------------------------------
