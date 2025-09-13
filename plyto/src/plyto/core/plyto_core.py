"""This Module provides Classes that represent a Pluto Core Instance.

Pluto Core manages Pluto Nodes.
"""
# ---------------------------------------------------------------------------------------------------------------------
from typing import Self, List, Dict
from json import dumps, loads
from logging import Logger
from plyto.core.plyto_node import PlytoNode
from typing import Self, Any
from plyto.config.plyto_core_config import PlytoCoreConfig
from plyto.config.plyto_config import PlytoConfig
from os import environ

# ---------------------------------------------------------------------------------------------------------------------


class PlytoCore:
    """A PlytoCore Object."""
    def __init__(
        self,
        workdir: str,
        logger: Logger
    ):
        """C'tor."""
        self.__logger: Logger = logger.getChild(self.__class__.__name__)
        self.__workdir: str = workdir
        self.__nodes = {}
        pass
    
    def add(self, node: PlytoNode) -> Self:
        """Add a Node to this Core Object.

        You can only add a Node once.

        Raises:
            ValueError: If the Node to be added is already part of this Object.
        """
        if node.name() in self.__nodes:
            raise ValueError(f"Name {node.name()} already exists.")
         
        self.__nodes[node.name()] = node
        node.set_workdir(self.__workdir)
        return self

    def connect(self, node0: PlytoNode, node1: PlytoNode) -> Self:
        """Connect two Nodes.
        
        Creates a Connection from node0 to node1.

        Raises:
            ValueError: If at least one of the given Nodes is not known by this Object.  
        """
        if node0.name() in self.__nodes and node1.name() in self.__nodes:
            node0.add_name_of_output_queue(node1.name_of_input_queue())
            return self
        raise ValueError(f"One or both given Nodes are not managed by this Object.")
    
    def nodes(self) -> Dict[str, PlytoNode]:
        """Get a List of known Nodes."""
        return self.__nodes
    
    def config(self, workdir: str) -> PlytoCoreConfig:
        """Get a Configuration that describes this Object."""
        config: PlytoCoreConfig = PlytoCoreConfig(workdir=workdir)
        for node in self.__nodes:
            config.add(self.__nodes[node].config())
        return config

    def exec(self) -> None:
        """Execute.

        This Function replaces the current Executable with pluto_core and runs it.
        """
        from os import execvp
        
        try:
            existing_config: PlytoCoreConfig = PlytoConfig.from_file(
                PlytoConfig.core_config_absolut_path(),
            )
            
            new_config: PlytoCoreConfig = self.config(
                workdir=PlytoConfig.read_workdir()
            )
        
            if not existing_config.equals(new_config):
                raise AssertionError('A Configuration already exists...')
        
        except FileNotFoundError:
            new_config: PlytoCoreConfig = self.config(
                workdir=PlytoConfig.read_workdir()
            )
            PlytoConfig.to_file(
                new_config,
            )

        execvp(
            f'{PlytoConfig.read_binarydir()}pluto_core',
            (
                f'{PlytoConfig.read_binarydir()}pluto_core',
                "-c",
                PlytoConfig.core_config_absolut_path(),
            ),
        )
        pass

    def to_string(self) -> str:
        """Returns a Repraesentation of this Object."""
        return {
            "work_dir": self.__workdir,
            "nodes": [self.__nodes[node].to_string() for node in self.__nodes],
        }

    pass


# ---------------------------------------------------------------------------------------------------------------------
