
from typing import Self, List
from json import dumps, loads
from logging import Logger
from plyto.run.plyto_node import PlytoNode, PlytoNodeConfig
from typing import Self, Any
from plyto.config.plyto_node_config import PlytoNodeConfig
from plyto.config.plyto_core_config import PlytoCoreConfig
from os import environ


class PlytoConfig:

    def __init__(self):
        pass

    @staticmethod
    def core_config_file(self) -> str:
        return 'core.txt'
    
    @staticmethod
    def core_config_absolut_path(self) -> str:
        return f'{read_workdir()}{}'

    @staticmethod
    def read_workdir() -> str:
        return environ.get('PLUTO_WORKDIR', '/tmp/pluto/workdir/')
    
    @staticmethod
    def read_binarydir() -> str:
        return environ.get('PLUTO_BINARYDIR', '/usr/local/bin/')
    
    @staticmethod
    def from_file(filename: str) -> PlytoCoreConfig:
        with open(filename, "r") as file: 
            content: str = file.read()
            config: PlytoCoreConfig = PlytoCoreConfig.from_dict(loads(content))
        return PlytoCore(logger=logger.getChild(PlytoCore.__name__))

    pass


