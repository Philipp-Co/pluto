"""HTTP interface for connecting two nodes."""

# ----------------------------------------------------------------------------------------------------------------------------------------------

from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework.request import Request
from rest_framework.serializers import Serializer, CharField, BooleanField
from drf_spectacular.utils import extend_schema
from http import HTTPStatus
from logging import Logger, getLogger
from json import loads, JSONDecodeError
from pluto.domain.manage.core import Core, CoreState
from pluto.domain.manage.node import Node as DomainNode


# ----------------------------------------------------------------------------------------------------------------------------------------------


class ConnectNodeRequestSerializer(Serializer):
    """Deserializes a connect-node request.

    Fields:
        source_name: The name of the source node.
        target_name: The name of the target node.
    """

    source_name = CharField(max_length=64)
    target_name = CharField(max_length=64)

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


class ConnectNodeResponseSerializer(Serializer):
    """Serializes the result of a connect-node operation.

    Fields:
        result: Indicates whether the operation was successful.
        description: Human-readable description of the outcome.
    """

    result      = BooleanField()
    description = CharField(max_length=64)

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


class ConnectNodeView(APIView):
    """API view for connecting two nodes."""

    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)
        self.__logger: Logger = getLogger(self.__class__.__name__)
        pass

    @extend_schema(
        description='Connects two nodes. Requires the core process to be in the INITIAL state. The core process can be stopped via the runtime control HTTP endpoint.',
        request=ConnectNodeRequestSerializer,
        responses={
            HTTPStatus.OK.value: ConnectNodeResponseSerializer,
            HTTPStatus.BAD_REQUEST.value: ConnectNodeResponseSerializer,
            HTTPStatus.CONFLICT.value: ConnectNodeResponseSerializer,
            HTTPStatus.SERVICE_UNAVAILABLE.value: ConnectNodeResponseSerializer,
            HTTPStatus.INTERNAL_SERVER_ERROR.value: None,
        }
    )
    def put(self, request: Request) -> Response:
        """Connects two nodes.

        Args:
            request: The HTTP request containing a JSON body with source_name and target_name.

        Returns:
            A Response containing the serialized ConnectNodeResponse.
            Returns HTTP 400 if the request body is missing or invalid.
            Returns HTTP 409 if the core process is not in the INITIAL state.
            Returns HTTP 503 if the connection could not be established.
            Returns HTTP 500 if an unexpected error occurs.
        """
        try:
            if request.body is None:
                return Response(
                    status=HTTPStatus.BAD_REQUEST,
                    data={'result': False, 'description': 'Missing request body.'}
                )
            payload: str = request.body.decode()
            try:
                serializer: ConnectNodeRequestSerializer = ConnectNodeRequestSerializer(data=loads(payload))
                if not serializer.is_valid():
                    return Response(
                        status=HTTPStatus.BAD_REQUEST,
                        data={'result': False, 'description': 'Invalid request body.'}
                    )
            except JSONDecodeError:
                return Response(
                    status=HTTPStatus.BAD_REQUEST,
                    data={'result': False, 'description': 'Invalid request body.'}
                )
            if Core(self.__logger).state() != CoreState.INITIAL:
                return Response(
                    status=HTTPStatus.CONFLICT,
                    data={
                        'result': False,
                        'description': 'Core is not stopped.',
                    }
                )
            source_name: str = serializer.validated_data['source_name']
            target_name: str = serializer.validated_data['target_name']
            if not DomainNode(source_name, self.__logger).connect_to_node(target_name):
                return Response(
                    status=HTTPStatus.SERVICE_UNAVAILABLE,
                    data={
                        'result': False,
                        'description': 'Failed to connect nodes.',
                    }
                )
            return Response(
                status=HTTPStatus.OK,
                data={
                    'result': True,
                    'description': 'Nodes connected.',
                }
            )
        except Exception as e:
            self.__logger.exception(e)
            return Response(status=HTTPStatus.INTERNAL_SERVER_ERROR)

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------
