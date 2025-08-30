""""""
# ---------------------------------------------------------------------------------------------------------------------
from typing import Self, Any, Set
from plyto.config.plyto_node_config import PlytoNodeConfig, PlytoNodeType
from json import loads
# ---------------------------------------------------------------------------------------------------------------------

class PlytoCoreConfig:
    
    def __init__(self):
        self.__configs = []
        pass

    def add(self, node_config: PlytoNodeConfig) -> Self:
        self.__configs.append(node_config)
        return self
    
    @staticmethod
    def from_dict(content) -> 'PlytoCoreConfig':
        core_config: PlytoCoreConfig = PlytoCoreConfig()
        for config in content['nodes']:
            print(f'Parse {config}')
            with open(config['configuration-file'], 'r') as file:
                node_config: PlytoNodeConfig = PlytoNodeConfig.from_dict(
                    loads(
                        file.read(),
                    )
                ).set_type(
                    PlytoNodeType(config['type'])
                ).set_name(
                    config['name']
                )
                core_config.add(node_config)
        return core_config
    
    def node_configurations(self) -> Set[PlytoNodeConfig]:
        return set(
            self.__configs
        )

    def to_string(self) -> Any:
        return {
            'nodes': [
                {
                    'name': config.name(),
                    'type': config.type().name.lower(),
                    'configuration-file': config.configuration_file_path(),
                    'executable': config.executable(),
                } for config in self.__configs
            ] 
        }
    pass
# ---------------------------------------------------------------------------------------------------------------------
