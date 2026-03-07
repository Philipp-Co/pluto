"""Domain logic for controlling the pluto_core process.

Provides the CoreState enumeration representing the possible states of the
pluto_core process, and the Core class which sends POSIX signals to the
running pluto_core process to trigger start and stop operations.
"""

# ----------------------------------------------------------------------------------------------------------------------

import fcntl
import json
from dataclasses import dataclass
from enum import Enum
from logging import Logger
from os import system
from pathlib import Path
from subprocess import check_output
from typing import List, Optional

# ----------------------------------------------------------------------------------------------------------------------


class CoreState(Enum):
    """Enumeration of possible states of the pluto_core process.

    Attributes:
        UNKNOWN:     The state could not be determined.
        INITIAL:     The initial state; also reached again after the process has been stopped.
        RUNNING:     The process is currently running.
        TERMINATING: The process is in the process of terminating.
        TERMINATED:  The process has fully terminated.
    """

    UNKNOWN = "UNKNOWN"
    INITIAL = "INITIAL"
    RUNNING = "RUNNING"
    TERMINATING = "TERMINATING"
    TERMINATED = "TERMINATED"

    pass


# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class NodeReferenceData:
    """Holds a reference to a node from the core configuration.

    Attributes:
        name:        The name of the node.
        config_file: Path to the node's configuration file.
    """

    name: str
    config_file: Path

    pass


# ----------------------------------------------------------------------------------------------------------------------


@dataclass
class CoreConfigData:
    """Holds the core configuration.

    Attributes:
        nodes: References to all registered nodes.
    """

    nodes: List[NodeReferenceData]

    pass


# ----------------------------------------------------------------------------------------------------------------------


class Core:
    """Controls the pluto_core process via POSIX signals.

    Resolves the PID of the running pluto_core process and sends the
    appropriate signal to start or stop it.
    """

    def __init__(self, logger: Logger) -> None:
        """Initializes the Core with the given logger.

        Args:
            logger: The logger instance used for logging.
        """
        self.__logger: Logger = logger.getChild(self.__class__.__name__)
        pass

    def _state_file(self) -> Path:
        """Returns the path to the pluto_core state file."""
        return Path("/pluto/core/state.pluto_state")

    def __get_pid(self) -> int:
        """Returns the PID of the running pluto_core process."""
        return int(check_output(["pidof", "pluto_core"]))

    def start(self) -> None:
        """Sends SIGUSR2 to the pluto_core process to trigger a start."""
        system(f"kill -USR2 {self.__get_pid()}")

    def stop(self) -> None:
        """Sends SIGUSR1 to the pluto_core process to trigger a stop."""
        system(f"kill -USR1 {self.__get_pid()}")

    def _config_file(self) -> Path:
        """Returns the path to the pluto_core configuration file."""
        return Path("/pluto/core/config/core.cfg")

    def config(self) -> Optional[CoreConfigData]:
        """Returns the current core configuration.

        Returns:
            A CoreConfigData containing the registered node names,
            or None if the configuration could not be retrieved.
        """
        try:
            data = json.loads(self._config_file().read_text(encoding="utf-8"))
            return CoreConfigData(
                nodes=[
                    NodeReferenceData(name=n["name"], config_file=Path(n["configuration-file"])) for n in data["nodes"]
                ],
            )
        except (OSError, json.JSONDecodeError, KeyError) as e:
            self.__logger.exception(e)
            return None

    def state(self) -> CoreState:
        """Returns the current state of the pluto_core process.

        Returns:
            The current CoreState, or CoreState.UNKNOWN if the state could
            not be determined.
        """
        try:
            with open(self._state_file(), "r", encoding="utf-8") as f:
                fcntl.flock(f, fcntl.LOCK_SH | fcntl.LOCK_NB)
                try:
                    return CoreState(f.read().rstrip("\x00").strip())
                except ValueError as e:
                    self.__logger.exception(e)
                    return CoreState.UNKNOWN
                finally:
                    fcntl.flock(f, fcntl.LOCK_UN)
        except OSError as e:
            self.__logger.exception(e)
            return CoreState.UNKNOWN

    pass


# ----------------------------------------------------------------------------------------------------------------------
