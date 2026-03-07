"""HTTP client for the Pluto management API."""

# ----------------------------------------------------------------------------------------------------------------------

from requests import Response
from requests import delete as requests_delete
from requests import get as requests_get
from requests import post as requests_post
from requests import put as requests_put

# ----------------------------------------------------------------------------------------------------------------------


class PlutoManagementClient:
    """Client for the Pluto management HTTP API.

    Abstracts all HTTP calls to the /manage/ endpoints as defined in the OpenAPI schema.
    """

    def __init__(self, base_url: str) -> None:
        """Initializes the client with the given base URL.

        Args:
            base_url: The base URL of the Pluto HTTP interface (e.g. 'http://localhost:10000').
        """
        self._base_url: str = base_url
        pass

    def get_core_state(self) -> Response:
        """Returns the current state of the pluto_core process.

        Returns:
            The HTTP response. On success (200), the body contains a CoreState JSON object.
        """
        return requests_get(f'{self._base_url}/manage/')

    def start_core(self) -> Response:
        """Starts the pluto_core process.

        Returns:
            The HTTP response. Returns 200 on success, 500 on error.
        """
        return requests_post(f'{self._base_url}/manage/')

    def stop_core(self) -> Response:
        """Stops the pluto_core process.

        Returns:
            The HTTP response. Returns 200 on success, 500 on error.
        """
        return requests_delete(f'{self._base_url}/manage/')

    def get_node_structure(self) -> Response:
        """Returns the current node structure including all nodes and connections.

        Returns:
            The HTTP response. On success (200), the body contains a NodeStructure JSON object.
        """
        return requests_get(f'{self._base_url}/manage/node/')

    def add_node(self, name: str, top_level_package_name: str) -> Response:
        """Creates a node with the given name.

        Args:
            name:                   The name of the node to create.
            top_level_package_name: The top-level Python package name of the node.

        Returns:
            The HTTP response. On success (200), the body contains a ManageResponse JSON object.
        """
        return requests_put(
            f'{self._base_url}/manage/node/add/{name}/',
            json={'top_level_package_name': top_level_package_name},
        )

    def delete_node(self, name: str) -> Response:
        """Deletes the node with the given name.

        Args:
            name: The name of the node to delete.

        Returns:
            The HTTP response. On success (200), the body contains a ManageResponse JSON object.
        """
        return requests_delete(f'{self._base_url}/manage/node/add/{name}/')

    def upload_archive(self, name: str, content: str) -> Response:
        """Uploads a base64-encoded package archive for the given node.

        Args:
            name:    The name of the node.
            content: The base64-encoded binary content of the .tar.gz archive.

        Returns:
            The HTTP response. On success (200), the body contains a ManageResponse JSON object.
        """
        return requests_put(
            f'{self._base_url}/manage/node/add/{name}/package/',
            json={'content': content},
        )

    def add_edge(self, name: str) -> Response:
        """Adds an edge to the node with the given name.

        Args:
            name: The name of the node.

        Returns:
            The HTTP response. On success (200), the body contains an AddEdgeResponse JSON object.
        """
        return requests_put(f'{self._base_url}/manage/node/{name}/edge/')

    def connect_nodes(self, source_name: str, target_name: str) -> Response:
        """Connects two nodes.

        Args:
            source_name: The name of the source node.
            target_name: The name of the target node.

        Returns:
            The HTTP response. On success (200), the body contains a ConnectNodeResponse JSON object.
        """
        return requests_put(
            f'{self._base_url}/manage/node/connect/',
            json={'source_name': source_name, 'target_name': target_name},
        )

    def disconnect_nodes(self, source_name: str, target_name: str) -> Response:
        """Removes the connection between two nodes.

        Args:
            source_name: The name of the source node.
            target_name: The name of the target node.

        Returns:
            The HTTP response. On success (200), the body contains a ConnectNodeResponse JSON object.
        """
        return requests_delete(
            f'{self._base_url}/manage/node/connect/',
            json={'source_name': source_name, 'target_name': target_name},
        )

    pass


# ----------------------------------------------------------------------------------------------------------------------
