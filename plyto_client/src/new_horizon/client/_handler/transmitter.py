"""
Handler for transmitting events to a Pluto instance via HTTP.
"""

# ----------------------------------------------------------------------------------------------------------------------

from datetime import timezone
from http import HTTPStatus
from json import dumps

from requests import Response, post  # pylint: disable=import-error
from typing_extensions import Self

from ...exceptions.exceptions import (  # pylint: disable=relative-beyond-top-level
    NewHorizonException,
    NewHorizonPayloadToLargeException,
    NewHorizonTimestampException,
)
from ...types.event import NewHorizonEvent  # pylint: disable=relative-beyond-top-level

# ----------------------------------------------------------------------------------------------------------------------


class NewHorizonTransmitter:
    """Handler for transmitting events to a Pluto instance via HTTP."""

    def __init__(self):
        self.__address: str = ""
        pass

    def __delf__(self):
        self.close()
        pass

    def connect(self, address: str) -> Self:
        """Connect to the Pluto server."""
        self.__address = address
        return self

    def close(self) -> Self:
        """Close the connection."""
        return self

    def transmit(self, name: str, event: NewHorizonEvent) -> None:
        """Transmit an event to the specified node."""
        try:
            if len(event.payload) > 48:
                raise NewHorizonPayloadToLargeException
            if event.timestamp.tzinfo is None or event.timestamp.tzinfo != timezone.utc:
                raise NewHorizonTimestampException
            response: Response = post(
                url=f"http://{self.__address}/runtime/events/",
                headers={
                    "Content-Type": "application/json",
                },
                data=dumps(
                    {
                        "id": event.id,
                        "event_id": event.event_id,
                        "timestamp": event.timestamp.isoformat(),
                        "payload": event.payload,
                        "target_node_name": name,
                    }
                ),
                allow_redirects=False,
            )

            if HTTPStatus.OK != response.status_code:
                raise NewHorizonException("Service Unavailable")
        except Exception as e:
            raise NewHorizonException from e

    pass


# ----------------------------------------------------------------------------------------------------------------------
