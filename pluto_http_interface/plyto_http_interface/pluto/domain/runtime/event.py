"""Domain model for a single node event."""

# ----------------------------------------------------------------------------------------------------------------------

from dataclasses import dataclass
from datetime import datetime

# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class NodeEvent:
    """Represents a single event received from a node.

    Attributes:
        id:         The sequential identifier of the event.
        event_id:   The type identifier of the event.
        timestamp:  The UTC timestamp at which the event was created.
        payload:    The raw binary payload of the event.
    """

    id: int
    event_id: int
    timestamp: datetime
    payload: str

    pass


# ----------------------------------------------------------------------------------------------------------------------
