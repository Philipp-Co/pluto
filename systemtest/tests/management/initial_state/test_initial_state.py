"""System tests for the initial state of the Pluto system."""

# ----------------------------------------------------------------------------------------------------------------------

from http import HTTPStatus

from requests import Response

from tests.utils.base import PlutoSystemtestBase

# ----------------------------------------------------------------------------------------------------------------------


class TestInitialState(PlutoSystemtestBase):
    """Tests that verify the expected initial state of Pluto after startup."""

    def test_core_is_in_initial_state(self) -> None:
        """Verifies that the core process is in the INITIAL state and no nodes are defined."""
        response: Response = self._management.get_core_state()
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertEqual(response.json()['state'], 'INITIAL')
        response = self._management.get_node_structure()
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self.assertEqual(response.json()['nodes'], ['pluto_http_edge'])
        self.assertEqual(response.json()['connections'], [])
        pass

    pass


# ----------------------------------------------------------------------------------------------------------------------
