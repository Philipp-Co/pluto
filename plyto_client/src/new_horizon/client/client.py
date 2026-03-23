"""
Main client class for connecting to and interacting with a Pluto instance.
"""

# ----------------------------------------------------------------------------------------------------------------------

from logging import Logger
from os import environ
from typing import Optional

from typing_extensions import Self

from ..exceptions import (  # pylint: disable=relative-beyond-top-level
    NewHorizonAddressNotSetException,
    NewHorizonException,
)
from ..types.event import NewHorizonEvent  # pylint: disable=unused-import, relative-beyond-top-level
from ._handler.manager import NewHorizonManager
from ._handler.receiver import NewHorizonReceiver
from ._handler.transmitter import NewHorizonTransmitter

# ----------------------------------------------------------------------------------------------------------------------


class NewHorizonClient:
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
            raise NewHorizonAddressNotSetException from e
        except Exception as e:
            raise NewHorizonException from e
        return self

    def close(self) -> Self:
        """Close the connection to the Pluto server."""
        try:
            pass
        except Exception as e:
            raise NewHorizonException from e
        return self

    def manager(self) -> NewHorizonManager:
        """Return a manager handler for administrative operations."""
        if len(self.__address) > 0:
            return NewHorizonManager().connect(self.__address)
        raise NewHorizonException("Not Connected!")

    def transmitter(self) -> NewHorizonTransmitter:
        """Return a transmitter handler for sending events."""
        if len(self.__address) > 0:
            return NewHorizonTransmitter().connect(self.__address)
        raise NewHorizonException("Not Connected!")

    def receiver(self) -> NewHorizonReceiver:
        """Return a receiver handler for receiving events."""
        if len(self.__address) > 0:
            return NewHorizonReceiver().connect(self.__address)
        raise NewHorizonException("Not Connected!")

    pass


# ----------------------------------------------------------------------------------------------------------------------
