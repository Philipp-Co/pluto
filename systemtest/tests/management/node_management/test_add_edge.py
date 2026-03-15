"""System tests for node management - connecting nodes with edges."""

# ----------------------------------------------------------------------------------------------------------------------

import base64
from http import HTTPStatus

from requests import Response

from tests.utils.base import PlutoSystemtestBase

# ----------------------------------------------------------------------------------------------------------------------

SOURCE_NODE_NAME: str = 'source_node'
TARGET_NODE_NAME: str = 'target_node'
TOP_LEVEL_PACKAGE_NAME: str = 'examplenode'

# ----------------------------------------------------------------------------------------------------------------------


class TestAddEdge(PlutoSystemtestBase):
    """Tests that verify two nodes can be connected via an edge."""

    def test_two_nodes_can_be_connected_via_edge(self) -> None:
        """Verifies that two nodes can be connected and the connection appears in the node structure."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')

        self._management.upload_archive(SOURCE_NODE_NAME, content)
        response: Response = self._management.add_node(SOURCE_NODE_NAME, TOP_LEVEL_PACKAGE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertTrue(response.json()['result'])

        self._management.upload_archive(TARGET_NODE_NAME, content)
        response = self._management.add_node(TARGET_NODE_NAME, TOP_LEVEL_PACKAGE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertTrue(response.json()['result'])

        response = self._management.add_edge(SOURCE_NODE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertTrue(response.json()['result'])

        response = self._management.connect_nodes(SOURCE_NODE_NAME, TARGET_NODE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertTrue(response.json()['result'])

        response = self._management.get_node_structure()
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertTrue(response.json()['result'])
        self.assertIn(SOURCE_NODE_NAME, response.json()['nodes'])
        self.assertIn(TARGET_NODE_NAME, response.json()['nodes'])
        connections: list = response.json()['connections']
        self.assertTrue(
            any(
                c['source'] == SOURCE_NODE_NAME and c['target'] == TARGET_NODE_NAME
                for c in connections
            )
        )
        pass

    def test_connect_nodes_fails_when_source_node_is_missing(self) -> None:
        """Verifies that connecting nodes is rejected when the source node does not exist."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')

        self._management.upload_archive(TARGET_NODE_NAME, content)
        self._management.add_node(TARGET_NODE_NAME, TOP_LEVEL_PACKAGE_NAME)

        response: Response = self._management.connect_nodes(SOURCE_NODE_NAME, TARGET_NODE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertFalse(response.json()['result'])
        pass

    def test_connect_nodes_fails_when_target_node_is_missing(self) -> None:
        """Verifies that connecting nodes is rejected when the target node does not exist."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')

        self._management.upload_archive(SOURCE_NODE_NAME, content)
        self._management.add_node(SOURCE_NODE_NAME, TOP_LEVEL_PACKAGE_NAME)
        self._management.add_edge(SOURCE_NODE_NAME)

        response: Response = self._management.connect_nodes(SOURCE_NODE_NAME, TARGET_NODE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertFalse(response.json()['result'])
        pass

    def test_connection_between_two_nodes_can_be_removed(self) -> None:
        """Verifies that an existing connection between two nodes can be successfully removed."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')

        self._management.upload_archive(SOURCE_NODE_NAME, content)
        self._management.add_node(SOURCE_NODE_NAME, TOP_LEVEL_PACKAGE_NAME)
        self._management.upload_archive(TARGET_NODE_NAME, content)
        self._management.add_node(TARGET_NODE_NAME, TOP_LEVEL_PACKAGE_NAME)
        self._management.add_edge(SOURCE_NODE_NAME)
        self._management.connect_nodes(SOURCE_NODE_NAME, TARGET_NODE_NAME)

        response: Response = self._management.get_node_structure()
        connections: list = response.json()['connections']
        self.assertTrue(
            any(
                c['source'] == SOURCE_NODE_NAME and c['target'] == TARGET_NODE_NAME
                for c in connections
            )
        )

        response = self._management.disconnect_nodes(SOURCE_NODE_NAME, TARGET_NODE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertTrue(response.json()['result'])

        response = self._management.get_node_structure()
        connections = response.json()['connections']
        self.assertFalse(
            any(
                c['source'] == SOURCE_NODE_NAME and c['target'] == TARGET_NODE_NAME
                for c in connections
            ),
            msg=f'Result ist {connections}'
        )
        pass

    def test_disconnect_nodes_fails_when_source_node_is_missing(self) -> None:
        """Verifies that disconnecting nodes is rejected when the source node does not exist."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')

        self._management.upload_archive(TARGET_NODE_NAME, content)
        self._management.add_node(TARGET_NODE_NAME, TOP_LEVEL_PACKAGE_NAME)

        response: Response = self._management.disconnect_nodes(SOURCE_NODE_NAME, TARGET_NODE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertFalse(response.json()['result'])
        pass

    def test_disconnect_nodes_fails_when_target_node_is_missing(self) -> None:
        """Verifies that disconnecting nodes is rejected when the target node does not exist."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')

        self._management.upload_archive(SOURCE_NODE_NAME, content)
        self._management.add_node(SOURCE_NODE_NAME, TOP_LEVEL_PACKAGE_NAME)

        response: Response = self._management.disconnect_nodes(SOURCE_NODE_NAME, TARGET_NODE_NAME)
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertFalse(response.json()['result'])
        pass

    pass


# ----------------------------------------------------------------------------------------------------------------------
