#
# ---------------------------------------------------------------------------------------------------------------------
#
from os import path, makedirs
from pathlib import Path
from typing import Self, List, Dict
from json import dumps, loads
from logging import Logger, getLogger
from typing import Self, Any
from plyto.config.plyto_node_config import PlytoNodeConfig
from plyto.config.plyto_core_config import PlytoCoreConfig
from os import environ

#
# ---------------------------------------------------------------------------------------------------------------------
#


class PlytoConfig:
    def __init__(self):
        pass

    @staticmethod
    def core_config_file() -> str:
        return "core.txt"

    @staticmethod
    def core_config_absolut_path() -> str:
        return f"{PlytoConfig.read_workdir()}{PlytoConfig.core_config_file()}"

    @staticmethod
    def read_workdir() -> str:
        return environ.get("PLUTO_WORKDIR", "/tmp/pluto/workdir/")

    @staticmethod
    def read_binarydir() -> str:
        return environ.get("PLUTO_BINARYDIR", "/usr/local/bin/")

    @staticmethod
    def generate_default(filename: str) -> None:
        with open(filename, "w+") as file:
            file.write(dumps(PlytoCoreConfig().to_string()))
        pass

    @staticmethod
    def from_file(filename: str) -> PlytoCoreConfig:
        with open(filename, "r") as file:
            content: str = file.read()
            return PlytoCoreConfig.from_dict(loads(content), workdir=PlytoConfig.read_workdir())
        raise RuntimeError

    @staticmethod
    def to_file(config: PlytoCoreConfig) -> None:
        print(f'Write Plyto Configuration Files to: {config.workdir()}')
        if path.isdir(config.workdir()):
            raise AssertionError(
                f"The Workingdirectory {config.workdir()} already exists."
            )
        
        makedirs(config.workdir())

        filename: str = PlytoConfig.core_config_absolut_path()
        if Path(filename).is_file():
            raise AssertionError("File already exists.")
        with open(filename, "w+") as file:
            file.write(dumps(config.to_string()))

        node_config: PlytoNodeConfig
        for node_config in config.node_configurations():
            PlytoConfig.node_to_file(
                filename=f'{PlytoConfig.read_workdir()}{node_config.name()}.txt',
                config=node_config,
            )
        pass

    @staticmethod
    def node_to_file(filename: str, config: PlytoNodeConfig) -> None:
        if Path(filename).is_file():
            raise AssertionError

        with open(filename, "w+") as file:
            file.write(dumps(config.to_string()))
        pass

    pass


#
# ---------------------------------------------------------------------------------------------------------------------
#
