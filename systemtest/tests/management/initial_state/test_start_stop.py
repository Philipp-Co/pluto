"""System tests for starting and stopping the Pluto core process."""

# ----------------------------------------------------------------------------------------------------------------------

from http import HTTPStatus

from requests import Response

from tests.utils.base import PlutoSystemtestBase

# ----------------------------------------------------------------------------------------------------------------------


class TestStartStop(PlutoSystemtestBase):
    """Tests that verify the core process can be started and stopped."""

    def test_core_can_be_started_and_stopped(self) -> None:
        """Verifies that the core process transitions to RUNNING and back to INITIAL."""
        response: Response = self._management.start_core()
        self.assertEqual(response.status_code, HTTPStatus.OK)
        response = self._management.get_core_state()
        self.assertEqual(response.json()['state'], 'RUNNING')
        response = self._management.stop_core()
        self.assertEqual(response.status_code, HTTPStatus.OK)
        self._wait_for_state('INITIAL')
        response = self._management.get_core_state()
        self.assertEqual(response.json()['state'], 'INITIAL')
        pass

    pass


# ----------------------------------------------------------------------------------------------------------------------
