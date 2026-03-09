"""This Module provides Classes to configure and run Pluto Node Objects."""
# ---------------------------------------------------------------------------------------------------------------------
from typing import Self, Set, List, Any
from json import dumps
from enum import Enum
from plyto._internal.plyto_venv import PlytoPythonInterpreter
from plyto.config.plyto_node_config import PlytoNodeConfig, PlytoNodeType
from plyto.core.plyto_core import PlytoCore, IManaged

# ---------------------------------------------------------------------------------------------------------------------

class PlytoNode(IManaged):
    """A PlytoNode Object.

    A Node executes User Code based on Events received through Plutos Node and Edge Network.
    """
    def __init__(self, core: PlytoCore, name: str) -> None:
        """C'tor."""
        self.__name: str = name
        self.__type: PlytoNodeType = PlytoNodeType.PASSTHROUGH
        self.__workdir: str = ""
        self.__name_of_input_queue: str = f"{name}-iq"
        self.__names_of_output_queues: Set[str] = set()
        self.__executable: str = None
        self.__core: PlytoCore = core
        self.__core.add(self)
        pass

    def __rshift__(self, other):
        self.connect(other) 
        return self

    def core(self) -> PlytoCore:
        return self.__core

    def connect(self, node: 'PlytoNode') -> Self:
        if self.core() != node.core():
            raise ValueError('The given Node is managed by another Core.')
        self.add_name_of_output_queue(node.name_of_input_queue())
        return self

    def input_queue(self) -> str:
        """Get the Name of the Input Queue for this Node."""
        return self.__name_of_input_queue

    def set_executable(self, executable: str) -> Self:
        """Set the absolute Path to the Executable File which is executed by the Running pluto_node.

        The Executable must point to one of the following files
            - a Python File with a Class which implementes the Pluto Interface
            - a Shared Library which implements the Pluto Interface
        """
        self.__executable = executable
        return self

    def executable(self) -> str:
        """Get the absolute Path to the executed File."""
        return self.__executable

    def set_type(self, t: PlytoNodeType) -> Self:
        """Set the Type of this Node."""
        self.__type = t
        return self

    def set_workdir(self, workdir: str) -> Self:
        """Set the Working Directory for this Node."""
        self.__workdir = workdir
        return self

    def workdir(self) -> str:
        return self.__workdir

    def name_of_input_queue(self) -> str:
        """Get the Name of this Nodes Inputqueue."""
        return self.__name_of_input_queue

    def add_name_of_output_queue(self, name: str) -> Self:
        """Add a new Reference to the Set of Outputqueues from this Node."""
        self.__names_of_output_queues.add(name)
        return self

    def name(self) -> str:
        """Get this Nodes Name."""
        return self.__name

    def to_string(self) -> str:
        """Returns a Repraesentation of this Node."""
        return {
            "work_dir": self.__workdir,
            "name_of_input_queue": self.name_of_input_queue(),
            "names_of_output_queues": list(self.__names_of_output_queues),
            "name": self.__name,
        }

    def config(self) -> PlytoNodeConfig:
        """Get a Configuration Object which describes this Node."""
        return (
            PlytoNodeConfig(
                name=self.name(),
                work_dir=self.__workdir,
                name_of_input_queue=self.__name_of_input_queue,
                names_of_output_queues=self.__names_of_output_queues,
            )
            .set_name(self.name())
            .set_type(self.__type)
            .set_executable(self.__executable)
        )

    pass


# ---------------------------------------------------------------------------------------------------------------------
