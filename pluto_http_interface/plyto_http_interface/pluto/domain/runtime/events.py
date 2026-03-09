"""Domain logic for node event streaming and processing.

Provides the NodeEvents class for managing a continuous SSE event stream,
including lock-based access control via flock and processing of incoming
node events. Also defines NodeEventResult, LockError, and related constants
and enumerations.
"""

# ----------------------------------------------------------------------------------------------------------------------

import base64
import fcntl
import json
from dataclasses import dataclass
from enum import IntEnum
from logging import Logger
from pathlib import Path
from subprocess import CalledProcessError, run
from typing import Generator, List

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
class NodeEventResult:
    """Result of a node event processing operation.

    Attributes:
        result:      Indicates whether the processing was successful.
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

    Provides a synchronous SSE generator that emits node events from the
    queue file. Access to the stream is controlled via an exclusive flock
    on a lock file. Incoming events are processed via process_event.
    """

    def __init__(self, logger: Logger, lock_path: str) -> None:
        """Initializes NodeEvents with the given logger and lock file path.

        Args:
            logger:    The logger instance used for logging.
            lock_path: Path to the file used for exclusive stream locking.
        """
        self.__running: bool = True
        self.__logger: Logger = logger.getChild(self.__class__.__name__)
        self.__lock_file = open(lock_path, "w", encoding="utf-8")  # pylint: disable=consider-using-with
        pass

    def __del__(self) -> None:
        """Closes the lock file when the object is garbage collected."""
        self.__logger.info("Destroy NodeEvents Object!")
        self.__lock_file.close()
        pass

    def lock_eventstream(self) -> None:
        """Acquires an exclusive non-blocking lock on the event stream.

        Raises:
            LockError: If the lock cannot be acquired.
        """
        try:
            fcntl.flock(self.__lock_file, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError as e:
            raise LockError("Failed to acquire eventstream lock.") from e

    def unlock_eventstream(self) -> None:
        """Releases the exclusive lock on the event stream."""
        self.__logger.info("Unlock Eventstream...")
        fcntl.flock(self.__lock_file, fcntl.LOCK_UN)

    def __get_event(self) -> List[str]:
        """Reads and clears all pending events from the queue file.

        Returns:
            A list of raw event strings read from the queue file,
            or an empty list if the file is unavailable or locked.
        """
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
        except Exception as e:  # pylint: disable=broad-exception-caught
            self.__logger.exception(e)
            return []

    def process_event(self, event: NodeEvent, name: str) -> NodeEventResult:
        """Processes a node event and returns the result.

        Args:
            event: The node event to process.
            name:  The edge name passed to plyto_edge via -n.

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
                    "-i",
                    str(event.id),
                    "-e",
                    str(event.event_id),
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
                    + "'",
                ],
                check=True,
            )
            return NodeEventResult(result=True, description="")
        except CalledProcessError as e:
            self.__logger.exception(e)
            return NodeEventResult(result=False, description=str(e))

    def stream(self) -> Generator[str, None, None]:
        """Yields a continuous stream of JSON-serialized node events.

        Emits available node events on each iteration. Sends a keepalive
        comment when no events are available. Releases the eventstream
        lock upon completion.

        Yields:
            JSON-serialized node event strings or SSE keepalive comments,
            each terminated with a double newline as required by the SSE protocol.
        """
        try:
            while self.__running:
                events: List[str] = self.__get_event()
                if len(events) == 0:
                    yield ": keepalive\n\n"
                else:
                    for element in events:
                        yield element + "\n\n"
        except Exception as e:  # pylint: disable=broad-exception-caught
            self.__logger.exception(e)
        finally:
            self.unlock_eventstream()

    @staticmethod
    def to_json(event: NodeEvent, payload: bytes) -> str:
        """Serializes a node event and its payload to a compact JSON string.

        Args:
            event:   The node event to serialize.
            payload: The raw binary payload to base64-encode.

        Returns:
            A compact JSON string representation of the event.
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
