"""HTTP interface for the node runtime.

Provides serializers for incoming node events and their processing results,
as well as the RuntimeNodeView which exposes a GET endpoint for SSE-based
event streaming and a POST endpoint for receiving and processing node events.
"""

# ----------------------------------------------------------------------------------------------------------------------

import base64
import json
from http import HTTPStatus
from logging import Logger, getLogger
from typing import Union

from django.http import StreamingHttpResponse
from drf_spectacular.utils import extend_schema
from rest_framework.request import Request
from rest_framework.response import Response
from rest_framework.serializers import BooleanField, CharField, DateTimeField, IntegerField, Serializer
from rest_framework.views import APIView

from pluto.domain.runtime.event import NodeEvent
from pluto.domain.runtime.events import LockError, NodeEventResult, NodeEvents

# ----------------------------------------------------------------------------------------------------------------------

LOCK_PATH: str = "/tmp/pluto_eventstream.lock"

# ----------------------------------------------------------------------------------------------------------------------


class RuntimeNodeEventSerializer(Serializer):  # pylint: disable=abstract-method
    """Deserializes an incoming node event from JSON.

    Fields:
        id:               Unique identifier of the node (0–4294967295).
        event_id:         Identifier of the event type (0–4294967295).
        timestamp:        ISO 8601 formatted timestamp of the event.
        payload:          Base64-encoded event payload. May be empty.
        target_node_name: The name of the target node.
    """

    id = IntegerField(min_value=0, max_value=4294967295)
    event_id = IntegerField(min_value=0, max_value=4294967295)
    timestamp = DateTimeField(input_formats=["iso-8601"])
    payload = CharField(allow_blank=True)
    target_node_name = CharField(max_length=64)

    pass


# ----------------------------------------------------------------------------------------------------------------------


class RuntimeNodeEventResultSerializer(Serializer):  # pylint: disable=abstract-method
    """Serializes the result of a node event processing operation.

    Fields:
        result: Indicates whether the operation was successful.
        description: Human-readable description of the outcome.
    """

    result = BooleanField()
    description = CharField()

    pass


# ----------------------------------------------------------------------------------------------------------------------


class RuntimeNodeView(APIView):
    """API view for the node runtime interface.

    Exposes a GET endpoint for SSE-based event streaming and a POST endpoint
    for receiving and processing incoming node events.
    """

    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)
        self.__logger: Logger = getLogger(self.__class__.__name__)
        pass

    @extend_schema(
        description="Streams node events to the client as a server-sent event (SSE) stream.",
        request=None,
        responses={
            (HTTPStatus.OK.value, "text/event-stream"): {"type": "string"},
            HTTPStatus.SERVICE_UNAVAILABLE.value: RuntimeNodeEventResultSerializer,
            HTTPStatus.INTERNAL_SERVER_ERROR.value: RuntimeNodeEventResultSerializer,
        },
    )
    def get(self, _request: Request) -> Union[StreamingHttpResponse, Response]:
        """Streams node events to the client as a server-sent event (SSE) stream.

        Args:
            request: The incoming HTTP request.

        Returns:
            A StreamingHttpResponse delivering a continuous SSE stream.
            Returns HTTP 503 if the eventstream is currently locked.
            Returns HTTP 500 if an unexpected error occurs during setup.
        """
        try:
            node_events: NodeEvents = NodeEvents(self.__logger, LOCK_PATH)
            node_events.lock_eventstream()
            response: StreamingHttpResponse = StreamingHttpResponse(
                node_events.stream(),
                content_type="text/event-stream",
            )
            response["Cache-Control"] = "no-cache"
            response["X-Accel-Buffering"] = "no"
            return response
        except LockError as e:
            self.__logger.exception(e)
            return Response(
                RuntimeNodeEventResultSerializer(
                    NodeEventResult(result=False, description="Eventstream is currently locked.")
                ).data,
                status=HTTPStatus.SERVICE_UNAVAILABLE,
            )
        except Exception as e:  # pylint: disable=broad-exception-caught
            self.__logger.exception(e)
            return Response(
                RuntimeNodeEventResultSerializer(
                    NodeEventResult(result=False, description="Unable to start stream.")
                ).data,
                status=HTTPStatus.INTERNAL_SERVER_ERROR,
            )

    @extend_schema(
        description="Processes an incoming node event and returns the result.",
        request=RuntimeNodeEventSerializer,
        responses={
            HTTPStatus.OK.value: RuntimeNodeEventResultSerializer,
            HTTPStatus.BAD_REQUEST.value: RuntimeNodeEventResultSerializer,
            HTTPStatus.INTERNAL_SERVER_ERROR.value: RuntimeNodeEventResultSerializer,
        },
    )
    def post(self, request: Request) -> Response:
        """Processes an incoming node event and returns the result.

        Args:
            request: The HTTP request containing the serialized node event as JSON body.

        Returns:
            A Response containing a RuntimeNodeEventResult with the processing outcome.
            Returns HTTP 400 if the request body is missing or invalid.
            Returns HTTP 500 if an unexpected error occurs during processing.
        """
        try:
            if not request.body:
                return Response(
                    RuntimeNodeEventResultSerializer(
                        NodeEventResult(result=False, description="Unable to parse Inputdata.")
                    ).data,
                    status=HTTPStatus.BAD_REQUEST,
                )
            serializer: RuntimeNodeEventSerializer = RuntimeNodeEventSerializer(data=json.loads(request.body))
            if not serializer.is_valid():
                return Response(
                    RuntimeNodeEventResultSerializer(
                        NodeEventResult(result=False, description="Unable to parse Inputdata.")
                    ).data,
                    status=HTTPStatus.BAD_REQUEST,
                )
            event: NodeEvent = NodeEvent(
                id=serializer.validated_data["id"],
                event_id=serializer.validated_data["event_id"],
                timestamp=serializer.validated_data["timestamp"],
                payload=base64.b64decode(serializer.validated_data["payload"]),
            )
            result: NodeEventResult = NodeEvents(self.__logger, LOCK_PATH).process_event(
                event,
                serializer.validated_data["target_node_name"],
            )
            return Response(RuntimeNodeEventResultSerializer(result).data)
        except Exception as e:  # pylint: disable=broad-exception-caught
            self.__logger.exception(e)
            return Response(
                RuntimeNodeEventResultSerializer(
                    NodeEventResult(result=False, description="Unable to parse Inputdata.")
                ).data,
                status=HTTPStatus.INTERNAL_SERVER_ERROR,
            )

    pass


# ----------------------------------------------------------------------------------------------------------------------
