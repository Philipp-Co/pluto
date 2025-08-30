
from typing import Self, Optional
from ctypes import byref, POINTER
from ctypes import CDLL, c_void_p, c_int, c_char_p, c_uint32
from plyto.edge import PlutoTimestamp, tm
import json
from plyto.config.plyto_node_config import PlytoNodeConfig
from plyto.config.plyto_core_config import PlytoCoreConfig
from typing import Set
from json import loads

class MessageQueueFullError(Exception):
    pass

class MessageQueueEmptyError(Exception):
    pass

class PlytoEvent:

    def __init__(self, cdll: CDLL):
        self.__cdll: CDLL = cdll
        self.__event_pointer: c_void_p = c_void_p(self.__cdll.PLUTO_CreateEvent())
        pass
    
    def __del__(self):
        if self.__event_pointer is not None:
            self.__cdll.PLUTO_DestroyEvent(
                byref(self.__event_pointer),
            )
        pass

    @staticmethod
    def create():
        from plyto.edge import CDLL, _lib_pluto_edge
        from datetime import datetime, UTC
        from time import time
        event: PlytoEvent = PlytoEvent(
            cdll=_lib_pluto_edge,
        )
        return event.set_timestamp(
            _lib_pluto_edge.PLUTO_TimeNow()
        )

    def set_id(self, id: int) -> Self:
        self.__cdll.PLUTO_EventSetId(
            self.__event_pointer,
            id
        )
        return self

    def set_event_id(self, event_id: int) -> Self:
        self.__cdll.PLUTO_EventSetEvent(
            self.__event_pointer,
            event_id,
        )
        return self

    def set_timestamp(self, timestamp: PlutoTimestamp) -> Self:
        self.__cdll.PLUTO_EventSetTimestamp(
            self.__event_pointer,
            timestamp,
        )
        return self
    
    def set_payload(self, msg: str) -> Self:
        from ctypes import c_size_t 
        self.__cdll.PLUTO_EventCopyBufferToPayload(
            self.__event_pointer,
            msg.encode(),
            c_size_t(len(msg)),
        )
        return self

    def __str__(self) -> str:
        from ctypes import create_string_buffer
        buffer = create_string_buffer(
            1024
        )
        self.__cdll.PLUTO_EventToBuffer(
            self.__event_pointer,
            buffer,
            len(buffer),
        )
        return buffer.raw.decode()
    
    def pointer(self) -> c_void_p:
        return self.__event_pointer

    pass


class PlytoEdge:
    """Create a Pluto Edge Object.

    This Object is able to send Events to a Pluto Node wich is connected with the Input Channel
    represented by this Edge Object.
    """

    def __create_logger(self, cdll: CDLL) -> c_void_p:
        from ctypes import create_string_buffer
        NAME: str = 'Edge'
        name = create_string_buffer(len(NAME)+1)
        name.value = NAME.encode()
        return cdll.PLUTO_CreateLogger(
            name
        )
    
    @staticmethod
    def read_core_config() -> PlytoCoreConfig:
        data: str
        with open('/tmp/pluto/workdir/core.txt', 'r') as file:
            data = file.read()
        return PlytoCoreConfig.from_dict(loads(data))

    def __find_node_by_name(self, node_name: str) -> Optional[PlytoNodeConfig]:
        for item in self.__core_config.node_configurations():
            if node_name == item.name():
                return item
        return None

    def __init__(self, node_name: str, queue_name: str, core_config: PlytoCoreConfig):
        from plyto.edge import _lib_pluto_edge, CDLL
        self.__dll: CDLL = _lib_pluto_edge
        from ctypes import create_string_buffer
        
        self.__core_config: PlytoCoreConfig = core_config
        
        node_config: Optional[PlytoNodeConfig] = self.__find_node_by_name(node_name)
        if node_config is None:
            raise ValueError(
                f'Known Nodes are: {self.nodes()}'
            )
        path: str = f'{node_config.workdir()}'

        if queue_name != node_config.name_of_input_queue() and queue_name not in node_config.names_of_output_queues():
            raise ValueError(
                f'Known Queues are: {node_config.name_of_input_queue()} / {node_config.names_of_output_queues()}'
            )

        path_buffer = create_string_buffer(len(path)+1)
        path_buffer.value = path.encode()
        name_buffer = create_string_buffer(len(queue_name)+1)
        name_buffer.value = queue_name.encode()
        
        self.__edge: c_void_p = c_void_p(
            self.__dll.PLUTO_EDGE_CreateEdge(
                path_buffer, name_buffer, 0x777, self.__create_logger(self.__dll)
            )
        )
        if self.__edge is None:
            raise AssertionError(
                f'Unable to create Edge with path:{path}", name: "{name}".'
            )
        pass

    def __del__(self):
        if self.__edge is not None:
            self.__dll.PLUTO_EDGE_DestroyEdge(
                byref(self.__edge)
            )
        pass

    def send(self, msg: str, id: int, event: int) -> Self:
        event: PlytoEvent = PlytoEvent.create().set_id(
            id
        ).set_event_id(
            event
        ).set_payload(
            msg
        )
        if not self.__dll.PLUTO_EDGE_EdgeSendEvent(
            self.__edge, event.pointer(),
        ):
            raise MessageQueueFullError
        return self
    
    def receive(self) -> str:
        from ctypes import POINTER
        event: PlytoEvent = PlytoEvent.create()
        if self.__dll.PLUTO_EDGE_EdgeReceiveEvent(
            self.__edge,
            event.pointer(),
        ):
            return self.__dll.PLUTO_EventPayload(event.pointer()).decode()
        raise MessageQueueEmptyError
         
    pass

