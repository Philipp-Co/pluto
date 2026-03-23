"""
Handler for receiving events from a Pluto instance via HTTP streaming.
"""

# ----------------------------------------------------------------------------------------------------------------------

from base64 import b64decode
from datetime import datetime
from json import loads
from typing import Iterator, Optional

from requests import Response, get  # pylint: disable=import-error
from typing_extensions import Self

from ...exceptions import PlytoNoEventAvailableExcpetion  # pylint: disable=relative-beyond-top-level
from ...types.event import PlytoEvent  # pylint: disable=relative-beyond-top-level

# ----------------------------------------------------------------------------------------------------------------------


class PlytoReceiver:
    """Handler for receiving events from a Pluto instance via HTTP streaming."""

    def __init__(self):
        self.__response: Optional[Response] = None
        self.__response_iter: Optional[Iterator[bytes]] = None
        self.__chunk = ""
        pass

    def __delf__(self):
        self.close()
        pass

    def connect(self, address: str) -> Self:
        """Connect to the Pluto server and open the event stream."""
        response: Response = get(f"http://{address}/runtime/events/", stream=True)
        self.__response = response
        self.__response_iter = response.iter_content(chunk_size=4096)
        return self

    def close(self) -> Self:
        """Close the event stream."""
        if self.__response is not None:
            self.__response.close()
        return self

    def receive(self) -> PlytoEvent:
        """Receive the next event from the stream."""
        try:
            if self.__response_iter is None:
                raise PlytoNoEventAvailableExcpetion
            chunk: bytes = next(self.__response_iter)
            self.__chunk = self.__chunk + chunk.decode()
            try:
                index = self.__chunk.index("\n\n")
                line = self.__chunk[0:index]
                self.__chunk = self.__chunk[index + 2 : len(self.__chunk)]

                if line.startswith(":"):
                    raise ValueError

                if line.startswith("data:"):
                    result = loads(b64decode(line[line.index("data:") + len("data:") : len(line)]))
                    return PlytoEvent(
                        id=result["id"],
                        event_id=result["event_id"],
                        timestamp=datetime.fromisoformat(result["timestamp"]),
                        payload=result["payload"],
                    )
                raise ValueError
            except ValueError as e:
                raise PlytoNoEventAvailableExcpetion from e
        except PlytoNoEventAvailableExcpetion as e:
            raise e
        except Exception as e:
            raise PlytoNoEventAvailableExcpetion from e

    pass


# ----------------------------------------------------------------------------------------------------------------------
