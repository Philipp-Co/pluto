
from dataclasses import dataclass
from datetime import datetime


@dataclass
class PlytoEvent:
    id: int
    event_id: int
    timestamp: datatime 
    payload: str
    pass

