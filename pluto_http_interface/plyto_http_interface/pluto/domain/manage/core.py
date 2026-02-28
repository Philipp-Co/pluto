"""Domain logic for controlling the pluto_core process.

Provides the CoreState enumeration representing the possible states of the
pluto_core process, and the Core class which sends POSIX signals to the
running pluto_core process to trigger start and stop operations.
"""

# ----------------------------------------------------------------------------------------------------------------------------------------------

import fcntl
from enum import Enum
from logging import Logger
from os import system
from pathlib import Path
from subprocess import check_output


# ----------------------------------------------------------------------------------------------------------------------------------------------


class CoreState(Enum):
    """Enumeration of possible states of the pluto_core process.

    Attributes:
        UNKNOWN:     The state could not be determined.
        INITIAL:     The initial state before any operation has been performed.
        STOPPED:     The process has been stopped.
        RUNNING:     The process is currently running.
        TERMINATING: The process is in the process of terminating.
        TERMINATED:  The process has fully terminated.
    """

    UNKNOWN     = 'UNKNOWN'
    INITIAL     = 'INITIAL'
    RUNNING     = 'RUNNING'
    TERMINATING = 'TERMINATING'
    TERMINATED  = 'TERMINATED'

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


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
        return Path('/pluto/core/state.pluto_state')

    def __get_pid(self) -> int:
        return int(check_output(['pidof', 'pluto_core']))

    def start(self) -> None:
        system(f'kill -USR2 {self.__get_pid()}')

    def stop(self) -> None:
        system(f'kill -USR1 {self.__get_pid()}')

    def state(self) -> CoreState:
        try:
            with open(self._state_file(), 'r') as f:
                fcntl.flock(f, fcntl.LOCK_SH | fcntl.LOCK_NB)
                try:
                    return CoreState(f.read().rstrip('\x00').strip())
                except ValueError as e:
                    self.__logger.exception(e)
                    return CoreState.UNKNOWN
                finally:
                    fcntl.flock(f, fcntl.LOCK_UN)
        except OSError as e:
            self.__logger.exception(e)
            return CoreState.UNKNOWN

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------
