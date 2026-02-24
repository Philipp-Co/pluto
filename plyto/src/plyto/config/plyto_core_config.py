""""""
# ---------------------------------------------------------------------------------------------------------------------
from typing import Any, Set
from typing_extensions import Self
from plyto.config.plyto_node_config import PlytoNodeConfig, PlytoNodeType
from json import loads, dumps

# ---------------------------------------------------------------------------------------------------------------------


class PlytoCoreConfig:
    def __init__(self, workdir: str):
        self.__configs = []
        self.__workdir: str = workdir
        pass
    
    def add(self, node_config: PlytoNodeConfig) -> Self:
        self.__configs.append(node_config)
        return self

    def workdir(self) -> str:
        return self.__workdir
    
    def equals(self, other: 'PlytoCoreConfig') -> bool:
        print(dir(other)) 
        if self.__workdir == other._PlytoCoreConfig__workdir:
            this = {dumps(config.to_string()) for config in self.__configs}
            otherc = {dumps(config.to_string()) for config in other._PlytoCoreConfig__configs}
            return this == otherc
        return False

    @staticmethod
    def from_dict(content, workdir: str) -> "PlytoCoreConfig":
        core_config: PlytoCoreConfig = PlytoCoreConfig(workdir=workdir)
        for config in content["nodes"]:
            with open(config["configuration-file"], "r") as file:
                node_config: PlytoNodeConfig = PlytoNodeConfig.from_dict(
                    name=config["name"],
                    content=loads(
                        file.read(),
                    ),
                ).set_type(PlytoNodeType(config["type"]))
                core_config.add(node_config)
        return core_config

    def node_configurations(self) -> Set[PlytoNodeConfig]:
        return set(self.__configs)

    def to_string(self) -> Any:
        return {
            "nodes": [
                {
                    "name": config.name(),
                    "type": config.type().name.lower(),
                    "configuration-file": config.configuration_file_path(),
                    "executable": config.executable(),
                }
                for config in self.__configs
            ]
        }

    pass


# ---------------------------------------------------------------------------------------------------------------------
