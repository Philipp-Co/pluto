""""""
# ---------------------------------------------------------------------------------------------------------------------
from typing import Self, List
from json import dumps, loads
from logging import Logger
from plyto.run.plyto_node import PlytoNode, PlytoNodeConfig
from typing import Self, Any
from plyto.config.plyto_node_config import PlytoNodeConfig
from plyto.config.plyto_core_config import PlytoCoreConfig
from os import environ

# ---------------------------------------------------------------------------------------------------------------------

class PlytoCore:

    def __init__(self, logger: Logger):
        self.__logger: Logger = logger.getChild(self.__class__.__name__)
        self.__nodes = {}
        self.__workdir: str = self.read_workdir()
        self.__binarydir: str = f'{self.read_binarydir()}pluto_core'
        pass

    def set_binarydir(self, binarydir: str) -> Self:
        self.__binarydir = binarydir
        return self

    def set_workdir(self, workdir: str) -> Self:
        self.__workdir = workdir
        return self
    
    def workdir(self) -> str:
        return self.__workdir

    def add(self, node: PlytoNode) -> Self:
        if node.name() in self.__nodes:
            raise ValueError(
                f'Name {node.name()} already exists.'
            )

        node.set_workdir(self.workdir())
        self.__nodes[node.name()] = node
        return self
    
    def connect(self, node0: PlytoNode, node1: PlytoNode) -> Self:
        if node0.name() in self.__nodes:
            node0.add_name_of_output_queue(node1.name_of_input_queue()) 
            return self
        raise ValueError(
            f'One or both given Nodes are not managed by this Object.'
        )

    def build_or_use_existing(self) -> Self:
        self.__logger.info(
            f'Build or use existing Configuration from {self.workdir()}...'
        )
        
        from os import path, mkdir
        if not path.isdir(self.workdir()):
            return self.build()
        
        return self

    def build(self) -> Self:
        self.__logger.info(
            'Build Configuration...'
        )
        
        from os import path, makedirs

        if path.isdir(self.workdir()):
            raise AssertionError(
                f'The Workingdirectory {self.workdir()} already exists.'
            )

        makedirs(self.workdir())
    
        for node in self.__nodes:
            with open(f'{self.workdir()}{self.__nodes[node].name()}.txt', 'w') as file:
                file.write(
                    dumps(
                        self.__nodes[node].config().to_string()
                    )
                )

        with open(f'{self.workdir()}core.txt', 'w') as file:
            file.write(
                dumps(
                    self.config().to_string()
                )
            )
            file.flush()

        self.__logger.info(
            'Build Configuration done.'
        )
        return self

    def config(self) -> PlytoCoreConfig:
        config: PlytoCoreConfig = PlytoCoreConfig()
        for node in self.__nodes:
            config.add(
                self.__nodes[node].config()
            )
        return config

    def node_configurations(self) -> List[PlytoNodeConfig]:
        return [
            self.__nodes[node].config() for node in self.__nodes
        ]
    
    def binary(self) -> str:
        return self.__binarydir

    def exec(self) -> None:
        self.__logger.info(
            f'Starting with Workingdirectory: {self.workdir()} and Binarydirectory: {self.binary()}'
        )
        from os import execvp
        execvp(
            self.binary(),
            (
                self.binary(), '-c', f'{self.workdir()}core.txt',
            ),
        )
        pass

    def to_string(self) -> str:
        return {
            'work_dir': self.__workdir,
            'nodes': [
                self.__nodes[node].to_string() for node in self.__nodes
            ]
        }

    pass
# ---------------------------------------------------------------------------------------------------------------------

