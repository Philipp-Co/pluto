"""
Data class representing a Pluto event.
"""

# ----------------------------------------------------------------------------------------------------------------------

from dataclasses import dataclass
from datetime import datetime

# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class PlytoEvent:
    """Represents a single event exchanged with a Pluto instance."""

    id: int
    event_id: int
    timestamp: datetime
    payload: str
    pass


# ----------------------------------------------------------------------------------------------------------------------
