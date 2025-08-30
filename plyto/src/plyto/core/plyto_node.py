""""""
# ---------------------------------------------------------------------------------------------------------------------
from typing import Self, Set, List, Any
from json import dumps
from enum import Enum
from plyto._internal.plyto_venv import PlytoPythonInterpreter
from plyto.config.plyto_node_config import PlytoNodeConfig, PlytoNodeType

# ---------------------------------------------------------------------------------------------------------------------

class PlytoNode:
    
    def __init__(self, name: str) -> None:
        self.__name: str = name
        self.__type: PlytoNodeType = PlytoNodeType.PASSTHROUGH
        self.__workdir: str = ''
        self.__name_of_input_queue: str = f'{name}-iq'
        self.__names_of_output_queues: Set[str] = set()
        self.__executable: str = None
        pass

    def input_queue(self) -> str:
        return self.__name_of_input_queue
   
    def set_executable(self, executable: str) -> Self:
        self.__executable = executable
        return self

    def executable(self) -> str:
        return self.__executable

    def set_type(self, t: PlytoNodeType) -> Self:
        self.__type = t
        return self

    def set_workdir(self, workdir: str) -> Self:
        self.__workdir = workdir
        return self
    
    def name_of_input_queue(self) -> str:
        return self.__name_of_input_queue

    def add_name_of_output_queue(self, name: str) -> Self:
        self.__names_of_output_queues.add(name)
        return self

    def name(self) -> str:
        return self.__name

    def to_string(self) -> str:
        return {
            'work_dir': self.__workdir,
            'name_of_input_queue': self.name_of_input_queue(),
            'names_of_output_queues': list(self.__names_of_output_queues),
            'name': self.__name,
        }

    def config(self) -> PlytoNodeConfig:
        return PlytoNodeConfig(
            work_dir=self.__workdir,
            name_of_input_queue=self.__name_of_input_queue,
            names_of_output_queues=self.__names_of_output_queues,
        ).set_name(
            self.name()
        ).set_type(
            self.__type
        ).set_executable(
            self.__executable
        )

    pass
# ---------------------------------------------------------------------------------------------------------------------

