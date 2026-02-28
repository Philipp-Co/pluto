"""Domain logic for managing node package archives."""

# ----------------------------------------------------------------------------------------------------------------------------------------------

from logging import Logger
from pathlib import Path
from subprocess import CalledProcessError, run

from typing_extensions import Self


# ----------------------------------------------------------------------------------------------------------------------------------------------


class Node:
    """Manages package archive files for nodes."""

    def __init__(self, name: str, logger: Logger) -> None:
        """Initializes the Node with the given name.

        Args:
            name: The name of the node.
            logger: The logger instance used for logging.
        """
        self.__name: str = name
        self.__logger: Logger = logger.getChild(self.__class__.__name__)
        pass

    def node_exists(self) -> bool:
        """Checks whether any content already exists for this node.

        Returns:
            True if the node exists, False otherwise.
        """
        pass

    def create_node(self, top_level_package_name: str) -> bool:
        """Creates the node with the given package configuration.

        Requires the archive file to be present. Returns False immediately
        if the archive file does not exist.

        Args:
            top_level_package_name: The top-level Python package name of the node.

        Returns:
            True if the node was created successfully, False otherwise.
        """
        try:
            if not self._temp_archive_file().exists():
                return False
            run(
                ['plyto_add_node_py', self.__name, str(self._temp_archive_file()), top_level_package_name],
                check=True
            )
            return True
        except CalledProcessError as e:
            self.__logger.exception(e)
            return False

    def connect_to_node(self, name: str) -> bool:
        """Connects this node to another node.

        Args:
            name: The name of the node to connect to.

        Returns:
            True if the connection was established successfully, False otherwise.
        """
        try:
            run(
                ['plyto_connect_nodes', self.__name, name],
                check=True
            )
            return True
        except CalledProcessError as e:
            self.__logger.exception(e)
            return False

    def archive_exists(self) -> bool:
        """Checks whether a package archive exists for this node.

        Returns:
            True if a package archive file exists, False otherwise.
        """
        return self._temp_archive_file().exists()

    def archive_write(self, content: bytes) -> bool:
        """Creates a new archive file and writes the given content to it.

        Args:
            content: The binary content to write to the archive file.

        Returns:
            True if the archive was written successfully, False otherwise.
        """
        try:
            self._temp_archive_directory().mkdir(parents=True, exist_ok=True)
            self._temp_archive_file().write_bytes(content)
            return True
        except OSError as e:
            self.__logger.exception(e)
            return False

    def archive_delete(self) -> Self:
        """Deletes the archive file of this node.

        Returns:
            This Node instance.
        """
        self._temp_archive_file().unlink(missing_ok=True)
        return self

    def _system_root_directory(self) -> Path:
        """Returns the root directory in which the system stores its directory structure.

        Returns:
            The path to the system root directory.
        """
        return Path('/pluto/')

    def _temp_archive_directory(self) -> Path:
        """Returns the directory for temporarily stored archive files.

        Returns:
            The path to the temporary archive directory.
        """
        return self._system_root_directory() / 'archive/'

    def _temp_archive_file(self) -> Path:
        """Returns the path to the temporarily stored archive file for this node.

        Returns:
            The path to the temporary archive file.
        """
        return self._temp_archive_directory() / f'{self.__name}.tar.gz'

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------
