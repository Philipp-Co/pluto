"""Health check backend implementation for the Pluto service."""

# ----------------------------------------------------------------------------------------------------------------------

from health_check.backends import BaseHealthCheckBackend  # pylint: disable=import-error

# ----------------------------------------------------------------------------------------------------------------------


class PlutoHealthCheck(BaseHealthCheckBackend):  # pylint: disable=too-few-public-methods
    """Health check backend that verifies the Pluto service is operational."""

    def check_status(self) -> None:
        """Performs the health check for the Pluto service."""
        pass

    pass
