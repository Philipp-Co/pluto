"""Django AppConfig for the health check interface."""

# ----------------------------------------------------------------------------------------------------------------------

from django.apps import AppConfig

# ----------------------------------------------------------------------------------------------------------------------


class HealthConfig(AppConfig):
    """AppConfig for the pluto health check interface.

    Registers PlutoHealthCheck with the django-health-check plugin registry
    once the application is ready.
    """

    name = "pluto.interfaces.health"

    def ready(self) -> None:
        """Registers health check plugins after Django initialization."""
        from health_check.plugins import plugin_dir  # pylint: disable=import-outside-toplevel,import-error

        from pluto.interfaces.health.checks import PlutoHealthCheck  # pylint: disable=import-outside-toplevel

        plugin_dir.register(PlutoHealthCheck)
        pass

    pass


# ----------------------------------------------------------------------------------------------------------------------
