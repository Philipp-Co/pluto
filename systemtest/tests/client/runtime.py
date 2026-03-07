"""HTTP client for the Pluto runtime API."""

# ----------------------------------------------------------------------------------------------------------------------

from requests import Response
from requests import get as requests_get
from requests import post as requests_post

# ----------------------------------------------------------------------------------------------------------------------


class PlutoRuntimeClient:
    """Client for the Pluto runtime HTTP API.

    Abstracts all HTTP calls to the /runtime/ endpoints as defined in the OpenAPI schema.
    """

    def __init__(self, base_url: str) -> None:
        """Initializes the client with the given base URL.

        Args:
            base_url: The base URL of the Pluto HTTP interface (e.g. 'http://localhost:10000').
        """
        self._base_url: str = base_url
        pass

    def process_event(self, node_id: int, event_id: int, timestamp: str, payload: str) -> Response:
        """Processes an incoming node event.

        Args:
            node_id:   Unique identifier of the node (0–4294967295).
            event_id:  Identifier of the event type (0–4294967295).
            timestamp: ISO 8601 formatted timestamp of the event.
            payload:   Base64-encoded event payload (may be empty).

        Returns:
            The HTTP response. On success (200), the body contains a RuntimeNodeEventResult JSON object.
        """
        return requests_post(
            f'{self._base_url}/runtime/events/',
            json={
                'id': node_id,
                'event_id': event_id,
                'timestamp': timestamp,
                'payload': payload,
            },
        )

    def stream_events(self) -> Response:
        """Opens a server-sent event (SSE) stream for node events.

        Returns:
            The HTTP response with Content-Type text/event-stream.
            The response is streamed and must be consumed iteratively.
        """
        return requests_get(f'{self._base_url}/runtime/events/', stream=True)

    pass


# ----------------------------------------------------------------------------------------------------------------------
