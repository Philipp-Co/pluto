
from pathlib import Path
from typing import Self, List
from json import dumps, loads
from logging import Logger, getLogger
from plyto.core.plyto_node import PlytoNode, PlytoNodeConfig
from typing import Self, Any
from plyto.config.plyto_node_config import PlytoNodeConfig
from plyto.config.plyto_core_config import PlytoCoreConfig
from os import environ


class PlytoConfig:

    def __init__(self):
        pass

    @staticmethod
    def core_config_file() -> str:
        return 'core.txt'
    
    @staticmethod
    def core_config_absolut_path() -> str:
        return f'{PlytoConfig.read_workdir()}{PlytoConfig.core_config_file()}'

    @staticmethod
    def read_workdir() -> str:
        return environ.get('PLUTO_WORKDIR', '/tmp/pluto/workdir/')
    
    @staticmethod
    def read_binarydir() -> str:
        return environ.get('PLUTO_BINARYDIR', '/usr/local/bin/')
    
    @staticmethod
    def generate_default(filename: str) -> None:
        with open(filename, "w+") as file:
            file.write(
                dumps(
                    PlytoCoreConfig().to_string()
                    
                )
            )
        pass

    @staticmethod
    def from_file(filename: str) -> PlytoCoreConfig:
        with open(filename, "r") as file: 
            content: str = file.read()
            return  PlytoCoreConfig.from_dict(loads(content))
        raise RuntimeError
    
    @staticmethod
    def to_file(filename: str, config: PlytoCoreConfig) -> None:
        if Path(filename).is_file():
            raise AssertionError(
                'File already exists.'
            )
        with open(filename, "w+") as file:
            file.write(
                dumps(
                    config.to_string()
                )
            )
        pass

    @staticmethod
    def node_to_file(filename: str, config: PlytoNodeConfig) -> None:
        if Path(filename).is_file():
            raise AssertionError

        with open(filename, "w+") as file:
            file.write(
                dumps(
                    config.to_string()
                )
            )
        pass

    pass


