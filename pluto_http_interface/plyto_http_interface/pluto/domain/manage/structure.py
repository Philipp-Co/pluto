"""Domain logic for querying the node structure."""

# ----------------------------------------------------------------------------------------------------------------------

from dataclasses import dataclass
from logging import Logger
from typing import List, Optional, Tuple

from pluto.domain.manage.core import Core
from pluto.domain.manage.node import Node

# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class NodeStructureData:
    """Holds the current node structure.

    Attributes:
        nodes: Names of all defined nodes.
        connections: Pairs of (source, target) node names representing connections.
    """

    nodes: List[str]
    connections: List[Tuple[str, str]]

    pass


# ----------------------------------------------------------------------------------------------------------------------


class NodeStructure:  # pylint: disable=too-few-public-methods
    """Queries the structure of all defined nodes."""

    def __init__(self, logger: Logger) -> None:
        """Initializes the NodeStructure with the given logger.

        Args:
            logger: The logger instance used for logging.
        """
        self.__logger: Logger = logger.getChild(self.__class__.__name__)
        pass

    def get(self) -> Optional[NodeStructureData]:
        """Returns the current node structure.

        Returns:
            A NodeStructureData containing node names and connections,
            or None if the structure could not be retrieved.
        """
        core_config = Core(self.__logger).config()
        if core_config is None:
            return None
        nodes = {ref.name: Node(ref.name, self.__logger).config() for ref in core_config.nodes}
        data: NodeStructureData = NodeStructureData(
            nodes=list(nodes.keys()),
            connections=[],
        )
        for name in nodes:
            node_config = nodes[name]
            if node_config is None:
                continue
            for target_node_name in nodes:
                if name != target_node_name:
                    target_config = nodes[target_node_name]
                    if target_config is None:
                        continue
                    if node_config.input_queue in target_config.output_queues:
                        data.connections.append((target_node_name, name))
        return data

    pass


# ----------------------------------------------------------------------------------------------------------------------
