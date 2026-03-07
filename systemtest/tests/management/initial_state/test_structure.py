"""System tests for the node structure endpoint."""

# ----------------------------------------------------------------------------------------------------------------------

from http import HTTPStatus

from requests import Response

from tests.utils.base import PlutoSystemtestBase

# ----------------------------------------------------------------------------------------------------------------------


class TestNodeStructure(PlutoSystemtestBase):
    """Tests for the GET /manage/node/ endpoint."""

    def test_get_node_structure(self) -> None:
        """Verifies that the node structure endpoint returns HTTP 200."""
        response: Response = self._management.get_node_structure()
        self.assertEqual(response.status_code, HTTPStatus.OK)
        pass

    pass


# ----------------------------------------------------------------------------------------------------------------------
