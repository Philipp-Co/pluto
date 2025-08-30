from unittest import TestCase
from pathlib import Path
from os import makedirs, removedirs, remove

from plyto.config.plyto_config import PlytoConfig
from plyto.config.plyto_core_config import PlytoCoreConfig
from plyto.config.plyto_node_config import PlytoNodeConfig

class Initial(TestCase):
    
    def setUp(self) -> None:
        """"""
        if not Path(PlytoConfig.read_workdir()).is_dir():
            makedirs(
                PlytoConfig.read_workdir()
            )
        PlytoConfig.generate_default(
            PlytoConfig.core_config_absolut_path()
        )
        pass
    
    def tearDown(self) -> None:
        """"""
        remove(PlytoConfig.core_config_absolut_path())
        pass

    def test_load_config(self) -> None:
        
        config: PlytoCoreConfig = PlytoConfig.from_file(
            PlytoConfig.core_config_absolut_path()
        )
        self.assertEqual(0, len(config.node_configurations()))

        pass

    pass


class NodeConfig(TestCase):
    pass

class CoreConfig(TestCase):
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.__config_file: Optional[str] = PlytoConfig.core_config_absolut_path()
        self.__node_config_files: List[str] = []
        pass

    def setUp(self) -> None:
        """"""
        if not Path(PlytoConfig.read_workdir()).is_dir():
            makedirs(
                PlytoConfig.read_workdir()
            )
        pass
    
    def tearDown(self) -> None:
        """"""
        if self.__config_file is not None:
            remove(self.__config_file)
        for item in self.__node_config_files:
            remove(item)

        pass

    def test_persist_empty(self) -> None:
        config: PlytoCoreConfig = PlytoCoreConfig()
        PlytoConfig.to_file(self.__config_file, config) 
        config = PlytoConfig.from_file(self.__config_file)
        self.assertEqual(0, len(config.node_configurations()))
        pass
    
    def test_persist_multiple_nodes(self) -> None:
        config: PlytoCoreConfig = PlytoCoreConfig()
        node_config_0: PlytoNodeConfig = PlytoNodeConfig(
            name='node0',
            work_dir=PlytoConfig.read_workdir(),
            name_of_input_queue='n0-iq',
            names_of_output_queues=['n0-oq'],
        ) 
        self.__node_config_files.append(node_config_0.configuration_file_path())
        PlytoConfig.node_to_file(
            node_config_0.configuration_file_path(),
            node_config_0
        )
        node_config_1: PlytoNodeConfig = PlytoNodeConfig(
            name='node1',
            work_dir=PlytoConfig.read_workdir(),
            name_of_input_queue='n1-iq',
            names_of_output_queues=['n1-oq'],
        ) 
        self.__node_config_files.append(node_config_1.configuration_file_path())
        PlytoConfig.node_to_file(
            node_config_1.configuration_file_path(),
            node_config_1
        )
        config.add(
            node_config_0
        )
        config.add(
            node_config_1
        )
        PlytoConfig.to_file(self.__config_file, config) 
        config = PlytoConfig.from_file(self.__config_file)
        self.assertEqual(2, len(config.node_configurations()))
        pass

    pass

