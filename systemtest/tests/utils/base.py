"""Base class for Pluto system tests."""

# ----------------------------------------------------------------------------------------------------------------------

import tempfile
from pathlib import Path
from subprocess import CompletedProcess
from subprocess import run as subprocess_run
from time import sleep, time
from unittest import TestCase

from tests.client.management import PlutoManagementClient
from tests.client.runtime import PlutoRuntimeClient
from tests.utils.constants import BASE_URL

# ----------------------------------------------------------------------------------------------------------------------


class PlutoSystemtestBase(TestCase):
    """Base class for all Pluto system tests.

    Starts a fresh Docker container before each test and tears it down afterwards.
    Provides pre-configured management and runtime clients for use in test cases.
    """

    def setUp(self) -> None:
        """Starts the Docker container and waits until it is healthy."""
        subprocess_run(['docker-compose', 'down', '-t', '0'], check=True)
        subprocess_run(['docker-compose', 'up', '-d'], check=True)
        self._wait_for_healthy()
        self._management: PlutoManagementClient = PlutoManagementClient(BASE_URL)
        self._runtime: PlutoRuntimeClient = PlutoRuntimeClient(BASE_URL)
        pass

    def tearDown(self) -> None:
        """Stops and removes the Docker container."""
        subprocess_run(['docker-compose', 'down'], check=True)
        pass

    def _wait_for_healthy(self, timeout: int = 120) -> None:
        """Waits until the Docker container reports a healthy status.

        Args:
            timeout: Maximum number of seconds to wait before raising a TimeoutError.

        Raises:
            TimeoutError: If the container does not become healthy within the given timeout.
        """
        start: float = time()
        while time() - start < timeout:
            result: CompletedProcess[str] = subprocess_run(
                ['docker-compose', 'ps', '-q', 'pluto'],
                capture_output=True,
                text=True,
                check=True,
            )
            container_id: str = result.stdout.strip()
            if container_id:
                health: CompletedProcess[str] = subprocess_run(
                    ['docker', 'inspect', '--format', '{{.State.Health.Status}}', container_id],
                    capture_output=True,
                    text=True,
                )
                if health.stdout.strip() == 'healthy':
                    return
            sleep(5)
        raise TimeoutError(f'Container did not become healthy within {timeout} seconds')

    @staticmethod
    def _build_examplenode_archive() -> bytes:
        """Builds the examplenode source distribution and returns its raw content.

        Runs `python -m build --sdist` in the examplenode directory and reads
        the resulting .tar.gz archive from a temporary output directory.

        Returns:
            The raw bytes of the built .tar.gz source distribution archive.
        """
        examplenode_dir: Path = (
            Path(__file__).parent.parent.parent.parent / 'pluto_internal_nodes' / 'examplenode'
        )
        with tempfile.TemporaryDirectory() as tmpdir:
            subprocess_run(
                ['python', '-m', 'build', '--sdist', '--outdir', tmpdir],
                cwd=str(examplenode_dir),
                check=True,
            )
            archives: list = list(Path(tmpdir).glob('*.tar.gz'))
            return Path(archives[0]).read_bytes()

    def _wait_for_state(self, expected_state: str, timeout: int = 30) -> None:
        """Waits until the core process reaches the expected state.

        Polls the core state endpoint every second until the expected state is
        reached or the timeout is exceeded.

        Args:
            expected_state: The state string to wait for (e.g. 'INITIAL', 'RUNNING').
            timeout:        Maximum number of seconds to wait before raising a TimeoutError.

        Raises:
            TimeoutError: If the expected state is not reached within the given timeout.
        """
        start: float = time()
        while time() - start < timeout:
            response = self._management.get_core_state()
            if response.json()['state'] == expected_state:
                return
            sleep(1)
        raise TimeoutError(f'Core did not reach state {expected_state!r} within {timeout} seconds')

    pass


# ----------------------------------------------------------------------------------------------------------------------
