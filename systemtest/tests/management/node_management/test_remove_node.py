"""System tests for node management - removing nodes."""

# ----------------------------------------------------------------------------------------------------------------------

import base64
import tempfile
from http import HTTPStatus
from pathlib import Path
from subprocess import run as subprocess_run

from requests import Response

from tests.utils.base import PlutoSystemtestBase

# ----------------------------------------------------------------------------------------------------------------------

EXAMPLENODE_DIR: Path = Path(__file__).parent.parent.parent.parent / 'examplenode'
SOURCE_NODE_NAME: str = 'source_node'
TARGET_NODE_NAME: str = 'target_node'
TOP_LEVEL_PACKAGE_NAME: str = 'examplenode'

# ----------------------------------------------------------------------------------------------------------------------


class TestRemoveNode(PlutoSystemtestBase):
    """Tests that verify a node can be removed."""

    @staticmethod
    def _build_examplenode_archive() -> bytes:
        """Builds the examplenode source distribution and returns its raw content.

        Returns:
            The raw bytes of the built .tar.gz source distribution archive.
        """
        with tempfile.TemporaryDirectory() as tmpdir:
            subprocess_run(
                ['python', '-m', 'build', '--sdist', '--outdir', tmpdir],
                cwd=str(EXAMPLENODE_DIR),
                check=True,
            )
            archives: list = list(Path(tmpdir).glob('*.tar.gz'))
            return Path(archives[0]).read_bytes()

    def _add_node(self, name: str, content: str) -> None:
        """Uploads the archive and adds a node as a test precondition.

        Args:
            name:    The name of the node to add.
            content: The base64-encoded archive content.
        """
        self._management.upload_archive(name, content)
        self._management.add_node(name, TOP_LEVEL_PACKAGE_NAME)

    def test_node_can_be_removed(self) -> None:
        """Verifies that a connected node can be removed and its connection is also removed."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')

        self._add_node(SOURCE_NODE_NAME, content)
        self._add_node(TARGET_NODE_NAME, content)
        self._management.add_edge(SOURCE_NODE_NAME)
        self._management.connect_nodes(SOURCE_NODE_NAME, TARGET_NODE_NAME)

        response: Response = self._management.delete_node(TARGET_NODE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertTrue(response.json()['result'])

        response = self._management.get_node_structure()
        self.assertIn(SOURCE_NODE_NAME, response.json()['nodes'])
        self.assertNotIn(TARGET_NODE_NAME, response.json()['nodes'])
        self.assertFalse(
            any(
                c['source'] == SOURCE_NODE_NAME and c['target'] == TARGET_NODE_NAME
                for c in response.json()['connections']
            )
        )

    def test_node_cannot_be_removed_when_not_in_initial_state(self) -> None:
        """Verifies that removing a node is rejected when the application is not in the INITIAL state."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')

        self._add_node(SOURCE_NODE_NAME, content)
        self._add_node(TARGET_NODE_NAME, content)
        self._management.start_core()

        response: Response = self._management.delete_node(TARGET_NODE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.CONFLICT)
        self.assertFalse(response.json()['result'])

    pass


# ----------------------------------------------------------------------------------------------------------------------
