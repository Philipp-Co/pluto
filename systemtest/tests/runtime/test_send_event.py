"""System tests for the runtime - sending node events."""

# ----------------------------------------------------------------------------------------------------------------------

import base64
import json
import threading
from datetime import datetime, timezone
from http import HTTPStatus
from time import sleep
from typing import List

from requests import Response

from tests.utils.base import PlutoSystemtestBase

# ----------------------------------------------------------------------------------------------------------------------

NODE_NAME: str = 'pluto_http_edge'
TOP_LEVEL_PACKAGE_NAME: str = 'examplenode'
EVENT_ID: int = 1

# ----------------------------------------------------------------------------------------------------------------------


class TestSendEvent(PlutoSystemtestBase):
    """Tests that verify a node event can be sent and received via SSE."""

    def test_event_can_be_sent_and_received_via_sse(self) -> None:
        """Verifies that a sent event is processed and received via the SSE stream."""
        archive: bytes = self._build_examplenode_archive()
        content: str = base64.b64encode(archive).decode('utf-8')
        self._management.upload_archive(NODE_NAME, content)
        self._management.add_node(NODE_NAME, TOP_LEVEL_PACKAGE_NAME)
        self._management.add_edge(NODE_NAME)
        self._management.start_core()
        self._wait_for_state('RUNNING')

        received_lines: List[str] = []
        stop_flag: threading.Event = threading.Event()

        def collect_stream() -> None:
            """Opens the SSE stream and collects non-comment event lines until stopped."""
            response = self._runtime.stream_events()
            for line in response.iter_lines():
                if stop_flag.is_set():
                    break
                if line:
                    decoded: str = line.decode('utf-8')
                    if decoded.startswith(':'):
                        continue
                    received_lines.append(decoded)

        thread = threading.Thread(target=collect_stream, daemon=True)
        thread.start()

        response: Response = self._runtime.process_event(
            node_id=1,
            event_id=EVENT_ID,
            timestamp=datetime.now(timezone.utc).isoformat(),
            payload=base64.b64encode(b'').decode('utf-8'),
            target_node_name=NODE_NAME,
        )
        self.assertEqual(response.status_code, HTTPStatus.OK, response.text)
        self.assertTrue(response.json()['result'], response.text)

        sleep(5)
        stop_flag.set()
        thread.join(timeout=10)
        self.assertEqual(len(received_lines), 1)
        try:
            event_data: dict = json.loads(received_lines[0])
        except json.JSONDecodeError as e:
            print(f'received_lines[0]: {received_lines[0]!r}')
            raise e
        self.assertEqual(event_data['event_id'], EVENT_ID)

    pass


# ----------------------------------------------------------------------------------------------------------------------
