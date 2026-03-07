"""System tests for node management - adding nodes and packages."""

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
NODE_NAME: str = 'example_node'
TOP_LEVEL_PACKAGE_NAME: str = 'examplenode'

# ----------------------------------------------------------------------------------------------------------------------


class TestAddNode(PlutoSystemtestBase):
    """Tests that verify a node can be added together with its package archive."""

    @staticmethod
    def _build_examplenode_archive() -> bytes:
        """Builds the examplenode source distribution and returns its raw content.

        Runs `python -m build --sdist` in the examplenode directory and reads
        the resulting .tar.gz archive from a temporary output directory.

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

    def test_node_can_be_added_with_package(self) -> None:
        """Verifies that a node can be added with a package archive and appears in the node structure."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')

        response: Response = self._management.upload_archive(NODE_NAME, content)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertTrue(response.json()['result'])
        response = self._management.get_core_state()
        self.assertEqual(response.json()['state'], 'INITIAL')

        response = self._management.add_node(NODE_NAME, TOP_LEVEL_PACKAGE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertTrue(response.json()['result'])
        response = self._management.get_core_state()
        self.assertEqual(response.json()['state'], 'INITIAL')

        response = self._management.get_node_structure()
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertTrue(response.json()['result'])
        self.assertIn(NODE_NAME, response.json()['nodes'])
        pass

    def test_node_cannot_be_added_without_archive(self) -> None:
        """Verifies that adding a node is rejected with 404 when no archive has been uploaded."""
        response: Response = self._management.add_node(NODE_NAME, TOP_LEVEL_PACKAGE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.NOT_FOUND)
        pass

    def test_node_cannot_be_added_twice_with_the_same_name(self) -> None:
        """Verifies that adding a second node with the same name is rejected."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')

        self._management.upload_archive(NODE_NAME, content)
        self._management.add_node(NODE_NAME, TOP_LEVEL_PACKAGE_NAME)

        self._management.upload_archive(NODE_NAME, content)
        response: Response = self._management.add_node(NODE_NAME, TOP_LEVEL_PACKAGE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertFalse(response.json()['result'])
        pass

    def test_node_cannot_be_added_when_not_in_initial_state(self) -> None:
        """Verifies that adding a node is rejected when the application is not in the INITIAL state."""
        response: Response = self._management.get_core_state()
        self.assertEqual(response.json()['state'], 'INITIAL')

        self._management.start_core()

        response = self._management.add_node(NODE_NAME, TOP_LEVEL_PACKAGE_NAME)
        self.assertFalse(response.json()['result'])
        pass

    pass


# ----------------------------------------------------------------------------------------------------------------------
