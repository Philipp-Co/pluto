"""
Handler for receiving events from a Pluto instance via HTTP streaming.
"""
# ----------------------------------------------------------------------------------------------------------------------

from base64 import b64decode
from typing_extensions import Self
from requests import get, Response
from plyto.client.event import PlytoEvent
from plyto.exceptions import PlytoNoEventAvailableExcpetion
from json import loads
from datetime import datetime

# ----------------------------------------------------------------------------------------------------------------------

class PlytoReceiver:

    def __init__(self):
        self.__response = None
        self.__response_iter: Generator[bytes, None, None] = None
        self.__chunk = ''
        pass

    def __delf__(self):
        self.close()
        pass

    def connect(self, address: str) -> Self:
        response: Response = get(f'http://{address}/runtime/events/', stream=True)
        self.__response = response
        self.__response_iter = response.iter_content(chunk_size=4096)
        return self

    def close(self) -> Self:
        self.__response.close()
        return self

    def receive(self) -> PlytoEvent:
        try:
            chunk: bytes = next(
                self.__response_iter
            )
            self.__chunk = self.__chunk + chunk.decode()
            try:
                index = self.__chunk.index('\n\n')
                line = self.__chunk[0: index]
                self.__chunk = self.__chunk[index + 2: len(self.__chunk)]

                if line.startswith(':'):
                    raise ValueError

                if line.startswith('data:'):
                    result = loads(
                        b64decode(
                            line[line.index('data:') + len('data:'): len(line)]
                        )
                    )
                    return PlytoEvent(
                        id=result['id'],
                        event_id=result['event_id'],
                        timestamp=datetime.fromisoformat(result['timestamp']),
                        payload=result['payload'],
                    )
                raise ValueError
            except ValueError as e:
                raise PlytoNoEventAvailableExcpetion from e
        except PlytoNoEventAvailableExcpetion as e:
            raise e
        except Exception as e:
            raise Exception from e

    pass

# ----------------------------------------------------------------------------------------------------------------------
