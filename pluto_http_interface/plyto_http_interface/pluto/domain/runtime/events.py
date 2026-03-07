"""Domain logic for node event streaming and processing.

Provides the NodeEvents class for managing a continuous SSE event stream,
including lock-based access control via flock, periodic status events,
and processing of incoming node events. Also defines NodeEventResult,
NodeStatusPayload, LockError, and related constants and enumerations.
"""

# ----------------------------------------------------------------------------------------------------------------------

import base64
import fcntl
import json
import time
from dataclasses import asdict, dataclass
from datetime import datetime, timezone
from enum import IntEnum
from logging import Logger
from pathlib import Path
from subprocess import CalledProcessError, run
from typing import Generator, List, Optional

from pluto.domain.runtime.event import NodeEvent

# ----------------------------------------------------------------------------------------------------------------------


class RuntimeNodeEventId(IntEnum):
    """Enumeration of reserved node event identifiers.

    Attributes:
        STATUS: Identifies a periodic status event.
        CLOSE: Identifies the final close event sent when the stream ends.
    """

    STATUS = 0x1000000
    CLOSE = 0x2000000

    pass


# ----------------------------------------------------------------------------------------------------------------------

GENERIC_ID: int = 0
STATUS_INTERVAL_SECONDS: float = 5.0

# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class NodeStatusPayload:
    """Payload of a periodic node status event.

    Attributes:
        active: Indicates whether the node is currently active.
        suspicious: Indicates whether the node has been flagged as suspicious.
    """

    active: bool
    suspicious: bool

    pass


# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class NodeEventResult:
    """Result of a node event processing operation.

    Attributes:
        result: Indicates whether the processing was successful.
        description: Human-readable description of the outcome.
    """

    result: bool
    description: str

    pass


# ----------------------------------------------------------------------------------------------------------------------


class LockError(Exception):
    """Raised when the eventstream lock cannot be acquired."""

    pass


# ----------------------------------------------------------------------------------------------------------------------


class NodeEvents:
    """Manages the node event stream and processes incoming node events.

    Provides a synchronous SSE generator that emits node events and periodic
    status events. Access to the stream is controlled via an exclusive flock
    on a lock file. Incoming events are processed via process_event.
    """

    def __init__(self, logger: Logger, lock_path: str) -> None:
        self.__running: bool = True
        self.__last_status_at: Optional[float] = None
        self.__logger: Logger = logger.getChild(self.__class__.__name__)
        self.__lock_file = open(lock_path, "w", encoding="utf-8")  # pylint: disable=consider-using-with
        pass

    def __del__(self):
        self.__logger.info("Destroy NodeEvents Object!")
        self.__lock_file.close()
        pass

    def lock_eventstream(self) -> None:
        """Acquires an exclusive non-blocking flock on the lock file.

        Raises:
            LockError: If the lock cannot be acquired.
        """
        try:
            fcntl.flock(self.__lock_file, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError as e:
            raise LockError("Failed to acquire eventstream lock.") from e

    def unlock_eventstream(self) -> None:
        """Releases the flock on the lock file."""
        self.__logger.info("Unlock Eventstream...")
        fcntl.flock(self.__lock_file, fcntl.LOCK_UN)

    def __get_event(self) -> List[str]:
        path: Path = Path("/pluto/nodes/http_edge/queue")
        try:
            if not path.exists():
                Path("/pluto/nodes/http_edge/").mkdir(parents=True, exist_ok=True)
                path.touch(exist_ok=True)
            lines: List[str] = []
            with open(path, "r+", encoding="utf-8") as file:
                fcntl.flock(file, fcntl.LOCK_EX | fcntl.LOCK_NB)
                lines = file.readlines()
                file.truncate(0)
                file.flush()
                fcntl.flock(file, fcntl.LOCK_UN)
            return lines
        except OSError as e:
            self.__logger.exception(e)
            return []

    def __get_status(self) -> NodeEvent:
        status: NodeStatusPayload = NodeStatusPayload(
            active=True,
            suspicious=False,
        )
        payload: bytes = json.dumps(asdict(status), separators=(",", ":")).encode("utf-8")
        return NodeEvent(
            id=GENERIC_ID,
            event_id=RuntimeNodeEventId.STATUS.value,
            timestamp=datetime.now(timezone.utc),
            payload=payload,
        )

    def process_event(self, event: NodeEvent, name: str) -> NodeEventResult:
        """Processes a node event and returns the result.

        Args:
            event: The node event to process.
            name: The edge name passed to plyto_edge via -n.

        Returns:
            A NodeEventResult containing the processing outcome and a description.
        """
        try:
            run(
                [
                    "plyto_edge",
                    "-n",
                    name,
                    "-w",
                    "-p",
                    "'"
                    + json.dumps(
                        {
                            "id": event.id,
                            "event_id": event.event_id,
                            "timestamp": event.timestamp.isoformat(),
                            "payload": event.payload.decode(),
                        }
                    )
                    + "'",  # base64.b64encode(event.payload).decode('utf-8')
                ],
                check=True,
            )
            return NodeEventResult(result=True, description="")
        except CalledProcessError as e:
            self.__logger.exception(e)
            return NodeEventResult(result=False, description=str(e))

    def stream(self) -> Generator[str, None, None]:
        """Yields a continuous stream of JSON-serialized node events.

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
                yielded: bool = False

                now: float = time.monotonic()
                if self.__last_status_at is None or now - self.__last_status_at >= STATUS_INTERVAL_SECONDS:
                    status: NodeEvent = self.__get_status()
                    yield NodeEvents.to_json(status, status.payload) + "\n\n"
                    self.__last_status_at = now
                    yielded = True

                events: List[str] = self.__get_event()
                for element in events:
                    yield element + "\n\n"
                    yielded = True

                if not yielded:
                    yield ": keepalive\n\n"
        except Exception as e:  # pylint: disable=broad-exception-caught
            self.__logger.exception(e)
        finally:
            self.unlock_eventstream()

    @staticmethod
    def to_json(event: NodeEvent, payload: bytes) -> str:
        """Serializes a node event and its payload to a JSON string.

        Args:
            event:   The node event to serialize.
            payload: The raw binary payload to include base64-encoded.

        Returns:
            A compact JSON string representing the event.
        """
        return json.dumps(
            {
                "id": event.id,
                "event_id": event.event_id,
                "timestamp": event.timestamp.isoformat(),
                "payload": base64.b64encode(payload).decode("utf-8"),
            },
            separators=(",", ":"),
        )

    pass


# ----------------------------------------------------------------------------------------------------------------------
