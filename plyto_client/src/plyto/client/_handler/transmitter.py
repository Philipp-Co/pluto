from http import HTTPStatus
from typing_extensions import Self
from plyto.client.event import PlytoEvent
from plyto.exceptions.exceptions import PlytoException
from datetime import timezone
from json import dumps
from requests import post, Response


class PlytoTransmitter:

    def __init__(self):
        self.__address: str = ''
        pass

    def __delf__(self):
        self.close()
        pass
    
    def connect(self, address: str) -> Self:
        self.__address = address
        return self
    
    def close(self) -> Self:
        return self

    def transmit(self, name: str, event: PlytoEvent) -> None:
        try:
            if len(event.payload) > 48:
                raise PlytoPayloadToLargeException
            if event.timestamp.tzinfo is None or event.timestamp.tzinfo != timezone.utc:
                raise PlytoTimestampException
            response: Response = post(
                    url=f'http://{self.__address}/runtime/events/',
                headers={
                    "Content-Type": "application/json",
                },
                data=dumps(
                    {
                        'id': event.id,
                        'event_id': event.event_id,
                        'timestamp': event.timestamp.isoformat(),
                        'payload': event.payload,
                        'target_node_name' : name,
                    }
                ),
                allow_redirects=False,
            ) 

            if HTTPStatus.OK != response.status_code:
                raise PlytoException('Service Unavailable')
        except Exception as e:
            raise PlytoException from e

    pass
