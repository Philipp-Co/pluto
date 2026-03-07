"""HTTP interface for connecting and disconnecting two nodes."""

# ----------------------------------------------------------------------------------------------------------------------

from http import HTTPStatus
from json import JSONDecodeError, loads

from drf_spectacular.utils import extend_schema, extend_schema_view
from rest_framework.request import Request
from rest_framework.response import Response
from rest_framework.serializers import BooleanField, CharField, Serializer

from pluto.domain.manage.node import Node as DomainNode
from pluto.interfaces.management.python.view import PlutoManageAPIView

# ----------------------------------------------------------------------------------------------------------------------


class ConnectNodeRequestSerializer(Serializer):  # pylint: disable=abstract-method
    """Deserializes a connect-node or disconnect-node request.

    Fields:
        source_name: The name of the source node.
        target_name: The name of the target node.
    """

    source_name = CharField(max_length=64)
    target_name = CharField(max_length=64)

    pass


# ----------------------------------------------------------------------------------------------------------------------


class ConnectNodeResponseSerializer(Serializer):  # pylint: disable=abstract-method
    """Serializes the result of a connect-node or disconnect-node operation.

    Fields:
        result:      Indicates whether the operation was successful.
        description: Human-readable description of the outcome.
    """

    result = BooleanField()
    description = CharField(max_length=64)

    pass


# ----------------------------------------------------------------------------------------------------------------------


@extend_schema_view(
    put=extend_schema(
        description=(
            "Connects two nodes. Requires the core process to be in the INITIAL state. "
            "The core process can be stopped via the runtime control HTTP endpoint."
        ),
        request=ConnectNodeRequestSerializer,
        responses={
            HTTPStatus.OK.value: ConnectNodeResponseSerializer,
            HTTPStatus.BAD_REQUEST.value: ConnectNodeResponseSerializer,
            HTTPStatus.CONFLICT.value: ConnectNodeResponseSerializer,
            HTTPStatus.SERVICE_UNAVAILABLE.value: ConnectNodeResponseSerializer,
            HTTPStatus.INTERNAL_SERVER_ERROR.value: None,
        },
    ),
    delete=extend_schema(
        description=(
            "Removes the connection between two nodes. Requires the core process to be in the INITIAL state. "
            "The core process can be stopped via the runtime control HTTP endpoint."
        ),
        request=ConnectNodeRequestSerializer,
        responses={
            HTTPStatus.OK.value: ConnectNodeResponseSerializer,
            HTTPStatus.BAD_REQUEST.value: ConnectNodeResponseSerializer,
            HTTPStatus.CONFLICT.value: ConnectNodeResponseSerializer,
            HTTPStatus.SERVICE_UNAVAILABLE.value: ConnectNodeResponseSerializer,
            HTTPStatus.INTERNAL_SERVER_ERROR.value: None,
        },
    ),
)
class ConnectNodeView(PlutoManageAPIView):
    """API view for connecting and disconnecting two nodes."""

    def handle_put(  # pylint: disable=unused-argument,too-many-return-statements
        self, request: Request, **kwargs
    ) -> Response:
        """Connects two nodes.

        Args:
            request: The HTTP request containing a JSON body with source_name and target_name.
            **kwargs: Additional URL parameters (unused).

        Returns:
            A Response containing the serialized ConnectNodeResponse.
            Returns HTTP 400 if the request body is missing or invalid.
            Returns HTTP 503 if the connection could not be established.
            Returns HTTP 500 if an unexpected error occurs.
        """
        try:
            if request.body is None:
                return Response(
                    status=HTTPStatus.BAD_REQUEST,
                    data={"result": False, "description": "Missing request body."},
                )
            payload: str = request.body.decode()
            try:
                serializer: ConnectNodeRequestSerializer = ConnectNodeRequestSerializer(data=loads(payload))
                if not serializer.is_valid():
                    return Response(
                        status=HTTPStatus.BAD_REQUEST,
                        data={"result": False, "description": "Invalid request body."},
                    )
            except JSONDecodeError:
                return Response(
                    status=HTTPStatus.BAD_REQUEST,
                    data={"result": False, "description": "Invalid request body."},
                )
            source_name: str = serializer.validated_data["source_name"]
            target_name: str = serializer.validated_data["target_name"]
            if not DomainNode(source_name, self._logger).node_exists():
                return Response(
                    status=HTTPStatus.OK,
                    data={"result": False, "description": "Source node not found."},
                )
            if not DomainNode(target_name, self._logger).node_exists():
                return Response(
                    status=HTTPStatus.OK,
                    data={"result": False, "description": "Target node not found."},
                )
            if not DomainNode(source_name, self._logger).connect_to_node(target_name):
                return Response(
                    status=HTTPStatus.SERVICE_UNAVAILABLE,
                    data={"result": False, "description": "Failed to connect nodes."},
                )
            return Response(
                status=HTTPStatus.OK,
                data={"result": True, "description": "Nodes connected."},
            )
        except Exception as e:  # pylint: disable=broad-exception-caught
            self._logger.exception(e)
            return Response(status=HTTPStatus.INTERNAL_SERVER_ERROR)

    def handle_delete(  # pylint: disable=unused-argument,too-many-return-statements
        self, request: Request, **kwargs
    ) -> Response:
        """Removes the connection between two nodes.

        Args:
            request: The HTTP request containing a JSON body with source_name and target_name.
            **kwargs: Additional URL parameters (unused).

        Returns:
            A Response containing the serialized ConnectNodeResponse.
            Returns HTTP 400 if the request body is missing or invalid.
            Returns HTTP 503 if the disconnection could not be performed.
            Returns HTTP 500 if an unexpected error occurs.
        """
        try:
            if request.body is None:
                return Response(
                    status=HTTPStatus.BAD_REQUEST,
                    data={"result": False, "description": "Missing request body."},
                )
            payload: str = request.body.decode()
            try:
                serializer: ConnectNodeRequestSerializer = ConnectNodeRequestSerializer(data=loads(payload))
                if not serializer.is_valid():
                    return Response(
                        status=HTTPStatus.BAD_REQUEST,
                        data={"result": False, "description": "Invalid request body."},
                    )
            except JSONDecodeError:
                return Response(
                    status=HTTPStatus.BAD_REQUEST,
                    data={"result": False, "description": "Invalid request body."},
                )
            source_name: str = serializer.validated_data["source_name"]
            target_name: str = serializer.validated_data["target_name"]
            if not DomainNode(source_name, self._logger).node_exists():
                return Response(
                    status=HTTPStatus.OK,
                    data={"result": False, "description": "Source node not found."},
                )
            if not DomainNode(target_name, self._logger).node_exists():
                return Response(
                    status=HTTPStatus.OK,
                    data={"result": False, "description": "Target node not found."},
                )
            if not DomainNode(source_name, self._logger).disconnect_from_node(target_name):
                return Response(
                    status=HTTPStatus.SERVICE_UNAVAILABLE,
                    data={"result": False, "description": "Failed to disconnect nodes."},
                )
            return Response(
                status=HTTPStatus.OK,
                data={"result": True, "description": "Nodes disconnected."},
            )
        except Exception as e:  # pylint: disable=broad-exception-caught
            self._logger.exception(e)
            return Response(status=HTTPStatus.INTERNAL_SERVER_ERROR)

    pass


# ----------------------------------------------------------------------------------------------------------------------
