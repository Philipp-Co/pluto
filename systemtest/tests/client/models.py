"""Domain models for the Pluto HTTP client, derived from the OpenAPI schema."""

# ----------------------------------------------------------------------------------------------------------------------

from dataclasses import dataclass
from typing import List

# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class CoreState:
    """Represents the current state of the pluto_core process.

    Attributes:
        state: The current state as a string (e.g. 'INITIAL', 'STARTED', 'STOPPED', 'HALTED').
    """

    state: str

    pass


# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class NodeConnection:
    """Represents a directed connection between two nodes.

    Attributes:
        source: The name of the source node.
        target: The name of the target node.
    """

    source: str
    target: str

    pass


# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class NodeStructure:
    """Represents the current node structure.

    Attributes:
        result:      Indicates whether the operation was successful.
        description: Human-readable description of the outcome.
        nodes:       Names of all defined nodes.
        connections: List of directed connections between nodes.
    """

    result: bool
    description: str
    nodes: List[str]
    connections: List[NodeConnection]

    pass


# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class ManageResponse:
    """Represents the result of a node management operation.

    Attributes:
        result:      Indicates whether the operation was successful.
        description: Human-readable description of the outcome.
    """

    result: bool
    description: str

    pass


# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class AddEdgeResponse:
    """Represents the result of an add-edge operation.

    Attributes:
        result:      Indicates whether the operation was successful.
        description: Human-readable description of the outcome.
    """

    result: bool
    description: str

    pass


# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class ConnectNodeResponse:
    """Represents the result of a connect-node operation.

    Attributes:
        result:      Indicates whether the operation was successful.
        description: Human-readable description of the outcome.
    """

    result: bool
    description: str

    pass


# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class RuntimeNodeEventResult:
    """Represents the result of a node event processing operation.

    Attributes:
        result:      Indicates whether the operation was successful.
        description: Human-readable description of the outcome.
    """

    result: bool
    description: str

    pass


# ----------------------------------------------------------------------------------------------------------------------
