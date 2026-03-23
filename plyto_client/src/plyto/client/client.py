"""
Main client class for connecting to and interacting with a Pluto instance.
"""

# ----------------------------------------------------------------------------------------------------------------------

from logging import Logger
from os import environ
from typing import Optional

from typing_extensions import Self

from ..exceptions import PlytoAddressNotSetException, PlytoException  # pylint: disable=relative-beyond-top-level
from ..types.event import PlytoEvent  # pylint: disable=unused-import, relative-beyond-top-level
from ._handler.manager import PlytoManager
from ._handler.receiver import PlytoReceiver
from ._handler.transmitter import PlytoTransmitter

# ----------------------------------------------------------------------------------------------------------------------


class PlytoClient:
    """Client for connecting to and interacting with a Pluto instance."""

    def __init__(self, logger: Optional[Logger] = None):
        """Initialize the client with an optional logger."""
        self.__address: str = ""
        self.__logger: Optional[Logger] = logger
        pass

    def __del__(self):
        pass

    def connect(self, address: Optional[str] = None, port: Optional[int] = None) -> Self:
        """Connect to the Pluto server. Falls back to PLYTO_ADDRESS and PLYTO_PORT env vars."""
        try:
            _address: str = environ["PLYTO_ADDRESS"] if address is None else address
            _port: int = int(environ["PLYTO_PORT"]) if port is None else port
            if self.__logger:
                self.__logger.info(f"Connect to Pluto ob Address {_address}:{_port}")
            self.__address = f"{_address}:{_port}"
        except KeyError as e:
            raise PlytoAddressNotSetException from e
        except Exception as e:
            raise PlytoException from e
        return self

    def close(self) -> Self:
        """Close the connection to the Pluto server."""
        try:
            pass
        except Exception as e:
            raise PlytoException from e
        return self

    def manager(self) -> PlytoManager:
        """Return a manager handler for administrative operations."""
        if len(self.__address) > 0:
            return PlytoManager().connect(self.__address)
        raise PlytoException("Not Connected!")

    def transmitter(self) -> PlytoTransmitter:
        """Return a transmitter handler for sending events."""
        if len(self.__address) > 0:
            return PlytoTransmitter().connect(self.__address)
        raise PlytoException("Not Connected!")

    def receiver(self) -> PlytoReceiver:
        """Return a receiver handler for receiving events."""
        if len(self.__address) > 0:
            return PlytoReceiver().connect(self.__address)
        raise PlytoException("Not Connected!")

    pass


# ----------------------------------------------------------------------------------------------------------------------
