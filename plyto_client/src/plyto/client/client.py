from base64 import b64encode
from json import loads, dumps
from typing_extensions import Self
from typing import List, Optional
from dataclasses import dataclass
from datetime import datetime, timezone
from os import environ
from logging import Logger

from plyto.exceptions import PlytoException, PlytoNoEventAvailableExcpetion, PlytoAddressNotSetException, PlytoTimestampException, PlytoPayloadToLargeException
from ._handler.transmitter import PlytoTransmitter
from ._handler.receiver import PlytoReceiver
from ._handler.manager import PlytoManager


@dataclass
class PlytoEvent:
    id: int
    event_id: int
    timestamp: datatime 
    payload: str
    pass


class PlytoClient:

    def __init__(self, logger: Optional[Logger] = None):
        self.__address: str = ''
        self.__logger: Optional[Logger] = logger
        pass

    def __del__(self):
        pass

    def connect(self, address: Optional[str] = None, port: Optional[int] = None) -> Self:
        try:
            _address: str = environ['PLYTO_ADDRESS'] if address is None else address
            _port: int = int(environ['PLYTO_PORT']) if port is None else port
            if self.__logger:
                self.__logger.info(
                    'Connect to Pluto ob Address %s:%s' % (_address, str(_port))
                )
            self.__address = f'{_address}:{_port}'
        except KeyError as e:
            raise PlytoAddressNotSetException from e
        except Exception as e:
            raise PlytoException from e
        return self

    def close(self) -> Self:
        try:
            pass
        except Exception as e:
            raise PlytoException from e
        return self

    def manager(self) -> PlytoManager:
        if len(self.__address) > 0:
            return PlytoManager().connect(self.__address)
        raise PlytoException('Not Connected!')

    def transmitter(self) -> PlytoTransmitter: 
        if len(self.__address) > 0:
            return PlytoTransmitter().connect(self.__address)
        raise PlytoException('Not Connected!')

    def receiver(self) -> PlytoReceiver:
        if len(self.__address) > 0:
            return PlytoReceiver().connect(self.__address)
        raise PlytoException('Not Connected!')

    pass

