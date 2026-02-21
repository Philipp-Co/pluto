"""PlytoEdge.

Connect to a Node inside the Plyto Universe.
"""
#
# -------------------------------------------------------------------------------------------------
#
from abc import ABC, abstractmethod
from typing import Self, Optional
from ctypes import create_string_buffer
from ctypes import byref, POINTER
from ctypes import CDLL, c_void_p, c_int, c_char_p, c_uint32
from plyto.edge import PLUTO_EDGE_Timestamp
import json
from plyto.config.plyto_config import PlytoConfig
from plyto.config.plyto_node_config import PlytoNodeConfig
from plyto.config.plyto_core_config import PlytoCoreConfig
from typing import Set
from json import loads
from datetime import datetime, UTC
from time import time

#
# -------------------------------------------------------------------------------------------------
#


class MessageQueueFullError(Exception):
    """Exception used by PlytoEdge."""
    pass


#
# -------------------------------------------------------------------------------------------------
#


class MessageQueueEmptyError(Exception):
    """Exception used by PlytoEdge."""
    pass


#
# -------------------------------------------------------------------------------------------------
#


class PlytoEvent:
    """A Pluto Event."""
    def __init__(self, cdll: CDLL):
        self.__cdll: CDLL = cdll
        self.__event_pointer: c_void_p = c_void_p(self.__cdll.PLUTO_EDGE_CreateEvent())
        pass

    def __del__(self):
        """D'tor."""
        if self.__event_pointer is not None:
            self.__cdll.PLUTO_EDGE_DestroyEvent(
                byref(self.__event_pointer),
            )
        pass

    @staticmethod
    def create() -> 'PlytoEvent':
        """Create a new Event.

        Prefere to use this Function to create a new Event over a normal call to the Constructor.

        Returns:
            PlytoEvent
        """
        from plyto.edge import CDLL, _lib_pluto_edge
        event: PlytoEvent = PlytoEvent(
            cdll=_lib_pluto_edge,
        )
        return event #.set_timestamp(_lib_pluto_edge.PLUTO_TimeNow())

    def set_id(self, id: int) -> Self:
        """Assign an Id to this Object."""
        self.__cdll.PLUTO_EDGE_EventSetId(self.__event_pointer, id)
        return self

    def set_event_id(self, event_id: int) -> Self:
        """Assign a Event Id to this Object."""
        self.__cdll.PLUTO_EDGE_EventSetEventId(
            self.__event_pointer,
            event_id,
        )
        return self

    """
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
    """

    def __str__(self) -> str:
        """To String Method."""
        buffer = create_string_buffer(1024)
        self.__cdll.PLUTO_EventToBuffer(
            self.__event_pointer,
            buffer,
            len(buffer),
        )
        return buffer.raw.decode()

    def pointer(self) -> c_void_p:
        """Get the C-Repraesentation of this Object."""
        return self.__event_pointer

    pass


#
# -------------------------------------------------------------------------------------------------
#

class PlytoEdge(ABC):
    """Create a Pluto Edge Object.

    This Object is able to send Events to a Pluto Node wich is connected with the Input Channel or
    to read from a Nodes Output represented by this Edge Object.
    """

    def __create_logger(self, cdll: CDLL) -> c_void_p:
        """Create a Logger in the C-World."""
        NAME: str = "Edge"
        name = create_string_buffer(len(NAME) + 1)
        name.value = NAME.encode()
        return cdll.PLUTO_CreateLogger(name)

    def __find_node_by_name(self, node_name: str) -> Optional[PlytoNodeConfig]:
        """Find a Nodeconfiguration given its Name."""
        for item in self.__core_config.node_configurations():
            if node_name == item.name():
                return item
        return None

    def __init__(self, node_name: str, queue_name: str, core_config: PlytoCoreConfig):
        """C'tor.

        Args:
            node_name: str: Name of the Node to which this Edge Object is connected.
            queue_name: str: Name of the Queue to which this Edge Object is connected.
            core_config: PlytoCoreConfig: Current Configuration. 
        """
        from plyto.edge import _lib_pluto_edge, CDLL
        self._dll: CDLL = _lib_pluto_edge
        
        self.__core_config: PlytoCoreConfig = core_config
        node_config: Optional[PlytoNodeConfig] = self.__find_node_by_name(node_name)
        if node_config is None:
            raise ValueError(f"Known Nodes are: {self.nodes()}")
        path: str = f"{node_config.workdir()}"

        if (
            queue_name != node_config.name_of_input_queue()
            and queue_name not in node_config.names_of_output_queues()
        ):
            raise ValueError(
                f"Known Queues are: {node_config.name_of_input_queue()} / {node_config.names_of_output_queues()}"
            )

        path_buffer = create_string_buffer(len(path) + 1)
        path_buffer.value = path.encode()
        name_buffer = create_string_buffer(len(queue_name) + 1)
        name_buffer.value = queue_name.encode()

        print(
            f'Create Edge Path={path}, Name={queue_name}'
        )
        self._edge: c_void_p = c_void_p(
            self._dll.PLUTO_EDGE_CreateEdge(
                path_buffer, name_buffer, 0x777, self.__create_logger(self._dll)
            )
        )
        if self._edge is None:
            raise AssertionError(
                f'Unable to create Edge with path:{path}", name: "{name}".'
            )
        pass

    def __del__(self):
        """D'tor."""
        if self._edge is not None:
            self._dll.PLUTO_EDGE_DestroyEdge(byref(self._edge))
        pass
    
    @abstractmethod
    def send(self, msg: str, id: int, event: int) -> Self:
        pass

    @abstractmethod
    def receive(self) -> str:
        pass

    pass

#
# -------------------------------------------------------------------------------------------------
#

class PlytoInputEdge(PlytoEdge):
    """A PlytoEdge which can be used to send Events to some Node.

    This Object is write only.
    """ 
    def send(self, msg: str, id: int, event: int) -> Self:
        event: PlytoEvent = (
            PlytoEvent.create().set_id(id).set_event_id(event)#.set_payload(msg)
        )
        if not self._dll.PLUTO_EDGE_EdgeSendEvent(
            self._edge,
            event.pointer(),
        ):
            raise MessageQueueFullError
        return self


    def receive(self) -> str:
        raise ValueError(
            'You are not allowed to read an Event from this Edge Object!'
        )

    pass

#
# -------------------------------------------------------------------------------------------------
#

class PlytoOutputEdge(PlytoEdge):
    """A PlytoEdge Object which ca be used to receive Events from some Node.

    This Object is read only.
    """ 
    def send(self, msg: str, id: int, event: int) -> Self:
        raise ValueError(
            'You are not allowed to write an Event to this Edge Object!'
        )

    def receive(self) -> str:
        event: PlytoEvent = PlytoEvent.create()
        if self._dll.PLUTO_EDGE_EdgeReceiveEvent(
            self._edge,
            event.pointer(),
        ):
            return self._dll.PLUTO_EventPayload(event.pointer()).decode()
        raise MessageQueueEmptyError
    pass

#
# -------------------------------------------------------------------------------------------------
#

class PlytoEdgeFactory:
    """A Factory that produces Edge Objects."""    

    @staticmethod
    def as_input_to_node(node_name: str) -> PlytoEdge:
        """Create an Edge Object, where its Output is connected to the Input Side of a Node.

        Args:
            node_name: str: The Nodename of the Node which reads Events from this Edge.

        Raises:
            ValueError: Raises a ValueError if the Nodename is not a Part of the Network given by the configuration in use.

        Returns:
            PlytoEdge
        """
        config: PlytoCoreConfig = PlytoConfig.from_file(
            PlytoConfig.core_config_absolut_path()
        )
        #
        # The given Node has to be part of the current Configuration.
        #
        item: PlytoNodeConfig
        for item in config.node_configurations():
            if item.name() == node_name:
                return PlytoInputEdge(
                    node_name=node_name,
                    queue_name=item.name_of_input_queue(),
                    core_config=config,
                )
        raise ValueError

    @staticmethod
    def as_output_from_node(node_name: str, queue_name: str) -> PlytoEdge:
        """Create an Edge Object where the Edge Objects Input in connectied to the Output of a given Node.

        Args:
            node_name: str: The Nodename of the Node which will write its Events to the Input of the specified Edge Object.
            queue_name: str: The Queuename where the Node will write its Events into.

        Returns:
            PlytoEdge
        """
        config: PlytoCoreConfig = PlytoConfig.from_file(
            PlytoConfig.core_config_absolut_path()
        )
        #
        # Check if "queue_name" is configured to be the Input Queue of some Node.
        # This must not be allowed, because Queue Implementations are based on the assumption, the there are
        # multiple Producers and exactely one Consumer.
        #
        if queue_name in {node_config.name_of_input_queue() for node_config in config.node_configurations()}:
            raise ValueError(
                'You can not create a PlytoEdge with a Queue which is configured to be some Nodes Input Queue.'
            )
        
        #
        # Make sure, the given Queuename exists in the Configuration.
        #
        list_of_lists = [node_config.names_of_output_queues() for node_config in config.node_configurations()] 
        if queue_name not in {j for i in list_of_lists for j in i}:
            raise ValueError(
                'Queue does not exist in current Configuration'
            )
        #
        # Construct EdgeObject.
        #
        return PlytoOutputEdge(
            node_name=node_name,
            queue_name=queue_name,
            core_config=config,
        )

    pass

#
# -------------------------------------------------------------------------------------------------
#
