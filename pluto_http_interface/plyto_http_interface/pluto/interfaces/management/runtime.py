"""HTTP interface for controlling the pluto_core process.

Provides the RuntimeControlView which exposes GET, POST and DELETE endpoints
for querying the current state of pluto_core and triggering start and stop
operations. Also defines CoreStateSerializer for serializing the process state.
"""

# ----------------------------------------------------------------------------------------------------------------------

from http import HTTPStatus
from logging import Logger, getLogger

from drf_spectacular.utils import extend_schema
from rest_framework.request import Request
from rest_framework.response import Response
from rest_framework.serializers import CharField, Serializer
from rest_framework.views import APIView

from pluto.domain.manage.core import Core, CoreState

# ----------------------------------------------------------------------------------------------------------------------


class CoreStateSerializer(Serializer):  # pylint: disable=abstract-method
    """Serializes the current state of the pluto_core process.

    Fields:
        state: The current state as a string value of CoreState.
    """

    state = CharField()

    pass


# ----------------------------------------------------------------------------------------------------------------------


class RuntimeControlView(APIView):
    """API view for controlling the pluto_core process.

    Exposes a GET endpoint for querying the current process state, a POST
    endpoint for starting the process, and a DELETE endpoint for stopping it.
    """

    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)
        self.__logger: Logger = getLogger(self.__class__.__name__)
        pass

    @extend_schema(
        description="Returns the current state of the pluto_core process.",
        request=None,
        responses={HTTPStatus.OK.value: CoreStateSerializer, HTTPStatus.INTERNAL_SERVER_ERROR.value: None},
    )
    def get(self, _request: Request) -> Response:
        """Returns the current state of the pluto_core process.

        Args:
            request: The incoming HTTP request.

        Returns:
            A Response containing the serialized CoreState.
            Returns HTTP 500 if an unexpected error occurs.
        """
        try:
            state: CoreState = Core(self.__logger).state()
            return Response(CoreStateSerializer({"state": state.value}).data)
        except Exception as e:  # pylint: disable=broad-exception-caught
            self.__logger.exception(e)
            return Response(status=HTTPStatus.INTERNAL_SERVER_ERROR)

    @extend_schema(
        description="Starts the pluto_core process.",
        request=None,
        responses={HTTPStatus.OK.value: None, HTTPStatus.INTERNAL_SERVER_ERROR.value: None},
    )
    def post(self, _request: Request) -> Response:
        """Starts the pluto_core process.

        Args:
            request: The incoming HTTP request.

        Returns:
            An empty Response with HTTP 200.
            Returns HTTP 500 if an unexpected error occurs.
        """
        try:
            Core(self.__logger).start()
            return Response()
        except Exception as e:  # pylint: disable=broad-exception-caught
            self.__logger.exception(e)
            return Response(status=HTTPStatus.INTERNAL_SERVER_ERROR)

    @extend_schema(
        description="Stops the pluto_core process.",
        request=None,
        responses={HTTPStatus.OK.value: None, HTTPStatus.INTERNAL_SERVER_ERROR.value: None},
    )
    def delete(self, _request: Request) -> Response:
        """Stops the pluto_core process.

        Args:
            request: The incoming HTTP request.

        Returns:
            An empty Response with HTTP 200.
            Returns HTTP 500 if an unexpected error occurs.
        """
        try:
            Core(self.__logger).stop()
            return Response()
        except Exception as e:  # pylint: disable=broad-exception-caught
            self.__logger.exception(e)
            return Response(status=HTTPStatus.INTERNAL_SERVER_ERROR)

    pass


# ----------------------------------------------------------------------------------------------------------------------
