"""Domain logic for node event streaming and processing.

Provides the NodeEvents class for managing a continuous SSE event stream,
including lock-based access control via flock, periodic status events,
and processing of incoming node events. Also defines NodeEventResult,
NodeStatusPayload, LockError, and related constants and enumerations.
"""

# ----------------------------------------------------------------------------------------------------------------------------------------------

from logging import Logger
import asyncio
import fcntl
import json
import base64
from dataclasses import dataclass, asdict
from datetime import datetime, timezone
from typing import AsyncGenerator, Optional
from enum import IntEnum
from pluto.domain.runtime.event import NodeEvent


# ----------------------------------------------------------------------------------------------------------------------------------------------


class RuntimeNodeEventId(IntEnum):
    """Enumeration of reserved node event identifiers.

    Attributes:
        STATUS: Identifies a periodic status event.
        CLOSE: Identifies the final close event sent when the stream ends.
    """

    STATUS = 0x1000000
    CLOSE  = 0x2000000

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------

GENERIC_ID: int              = 0
STATUS_INTERVAL_SECONDS: float = 5.0

# ----------------------------------------------------------------------------------------------------------------------------------------------


@dataclass
class NodeStatusPayload:
    """Payload of a periodic node status event.

    Attributes:
        active: Indicates whether the node is currently active.
        suspicious: Indicates whether the node has been flagged as suspicious.
    """

    active:     bool
    suspicious: bool

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


@dataclass
class NodeEventResult:
    """Result of a node event processing operation.

    Attributes:
        result: Indicates whether the processing was successful.
        description: Human-readable description of the outcome.
    """

    result:      bool
    description: str

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


class LockError(Exception):
    """Raised when the eventstream lock cannot be acquired."""

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


class NodeEvents:
    """Manages the node event stream and processes incoming node events.

    Provides an asynchronous SSE generator that emits node events and periodic
    status events. Access to the stream is controlled via an exclusive flock
    on a lock file. Incoming events are processed via process_event.
    """

    def __init__(self, logger: Logger, lock_path: str) -> None:
        self.__running: bool              = True
        self.__last_status_at: Optional[datetime] = None
        self.__logger: Logger             = logger.getChild(self.__class__.__name__)
        self.__lock_file                  = open(lock_path, 'w')
        pass

    def lock_eventstream(self) -> None:
        try:
            fcntl.flock(self.__lock_file, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError as e:
            raise LockError('Failed to acquire eventstream lock.') from e

    def unlock_eventstream(self) -> None:
        fcntl.flock(self.__lock_file, fcntl.LOCK_UN)

    def __get_event(self) -> Optional[NodeEvent]:
        return NodeEvent(
            id=GENERIC_ID,
            event_id=GENERIC_ID,
            timestamp=datetime.now(timezone.utc),
            payload=b'',
        )

    def __get_status(self) -> NodeEvent:
        status: NodeStatusPayload = NodeStatusPayload(
            active=True,
            suspicious=False,
        )
        payload: bytes = json.dumps(asdict(status), separators=(',', ':')).encode('utf-8')
        return NodeEvent(
            id=GENERIC_ID,
            event_id=RuntimeNodeEventId.STATUS.value,
            timestamp=datetime.now(timezone.utc),
            payload=payload,
        )

    def process_event(self, event: NodeEvent) -> NodeEventResult:
        """Processes a node event and returns the result.

        Args:
            event: The node event to process.

        Returns:
            A NodeEventResult containing the processing outcome and a description.
        """
        return NodeEventResult(
            result=True,
            description='',
        )

    async def stream(self) -> AsyncGenerator[str, None]:
        """Yields a continuous asynchronous stream of JSON-serialized node events.

        Emits available node events on each iteration and sends a periodic
        status event at the interval defined by STATUS_INTERVAL_SECONDS.
        Terminates with a final close event when the loop ends or an error
        occurs. Releases the eventstream lock upon completion.

        Yields:
            JSON-serialized node event strings, each terminated with a
            double newline as required by the SSE protocol.
        """
        try:
            while self.__running:
                try:
                    event: Optional[NodeEvent] = self.__get_event()
                    if event is not None:
                        data: str = NodeEvents.to_json(event, event.payload) + '\n\n'
                        yield data
                    now: datetime = datetime.now(timezone.utc)
                    if self.__last_status_at is None or (now - self.__last_status_at).total_seconds() >= STATUS_INTERVAL_SECONDS:
                        status_event: NodeEvent = self.__get_status()
                        self.__last_status_at = now
                        data: str = NodeEvents.to_json(status_event, status_event.payload) + '\n\n'
                        yield data
                    await asyncio.sleep(1)
                except Exception:
                    self.__running = False

            final_event: NodeEvent = NodeEvent(
                id=0,
                event_id=RuntimeNodeEventId.CLOSE.value,
                timestamp=datetime.now(timezone.utc),
                payload=b'',
            )
            final_data: str = NodeEvents.to_json(final_event, final_event.payload) + '\n\n'
            yield final_data
        finally:
            self.unlock_eventstream()

    @staticmethod
    def to_json(event: NodeEvent, payload: bytes) -> str:
        return json.dumps({
            'id':        event.id,
            'event_id':  event.event_id,
            'timestamp': event.timestamp.isoformat(),
            'payload':   base64.b64encode(payload).decode('utf-8'),
        }, separators=(',', ':'))

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------
