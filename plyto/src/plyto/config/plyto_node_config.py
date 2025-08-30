""""""
# ---------------------------------------------------------------------------------------------------------------------
from enum import Enum
from typing import Self, List, Any, Set
from plyto._internal.plyto_venv import PlytoPythonInterpreter
# ---------------------------------------------------------------------------------------------------------------------

class PlytoNodeType(Enum):
    PASSTHROUGH = "passthrough"
    PYTHON = "python"
    SHARED_LIBRARY = "shared"
    pass
# ---------------------------------------------------------------------------------------------------------------------

class PlytoNodeConfig:
    def __init__(self, name: str, work_dir: str, name_of_input_queue: str, names_of_output_queues: List[str]):
        self.__workdir: str = work_dir
        if self.__workdir[len(self.__workdir)-1] != '/':
            raise Value(f'The Working Directory must end with an "/".')
        self.__name_of_input_queue: str = name_of_input_queue
        self.__names_of_output_queues: List[str] = names_of_output_queues
        self.__name = name
        self.__type: PlytoNodeType = PlytoNodeType.PASSTHROUGH
        self.__executable: str = None
        self.__python_path: str = ';'.join(PlytoPythonInterpreter().python_path())
        pass
    
    @staticmethod
    def from_dict(content, name: str) -> 'PlytoNodeConfig':
        return PlytoNodeConfig(
            name=name,
            work_dir=content['work_dir'],
            name_of_input_queue=content['name_of_input_queue'],
            names_of_output_queues=content['names_of_output_queues'],
        )
    
    def name_of_input_queue(self) -> str:
        return self.__name_of_input_queue

    def names_of_output_queues(self) -> Set[str]:
        return set(
            self.__names_of_output_queues
        )

    def workdir(self) -> str:
        return self.__workdir

    def set_python_path(self, python_path: str) -> Self:
        self.__python_path = python_path
        return self

    def python_path(self) -> str:
        return self.__python_path
    
    def configuration_file_path(self) -> str:
        return f'{self.__workdir}{self.__name}.txt'
    
    def set_executable(self, executable: str) -> Self:
        self.__executable = executable
        return self

    def executable(self) -> str:
        return self.__executable

    def set_name(self, name: str) -> Self:
        self.__name = name
        return self

    def name(self) -> str:
        return self.__name

    def set_type(self, t: PlytoNodeType) -> Self:
        self.__type = t
        return self

    def type(self) -> PlytoNodeType:
        return self.__type

    def to_string(self) -> Any:
        if self.__name is None:
            raise ValueError('No Name!')
        return {
            'work_dir': self.__workdir,
            'name_of_input_queue': self.__name_of_input_queue,
            'names_of_output_queues': list(self.__names_of_output_queues),
        } | ({
            'python_path': self.__python_path,
        } if self.type() == PlytoNodeType.PYTHON else {})
    pass
# ---------------------------------------------------------------------------------------------------------------------
