"""HTTP interface for adding an edge to a node."""

# ----------------------------------------------------------------------------------------------------------------------

from http import HTTPStatus

from django.conf import settings
from drf_spectacular.utils import extend_schema, extend_schema_view
from rest_framework.request import Request
from rest_framework.response import Response
from rest_framework.serializers import BooleanField, CharField, Serializer

from pluto.domain.manage.node import Node as DomainNode
from pluto.interfaces.management.python.view import PlutoManageAPIView

# ----------------------------------------------------------------------------------------------------------------------


class AddEdgeResponseSerializer(Serializer):  # pylint: disable=abstract-method
    """Serializes the result of an add-edge operation.

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
            "Adds an edge to the node with the given name. " "Requires the core process to be in the INITIAL state."
        ),
        request=None,
        responses={
            HTTPStatus.OK.value: AddEdgeResponseSerializer,
            HTTPStatus.CONFLICT.value: AddEdgeResponseSerializer,
            HTTPStatus.SERVICE_UNAVAILABLE.value: AddEdgeResponseSerializer,
            HTTPStatus.INTERNAL_SERVER_ERROR.value: None,
        },
    ),
)
class AddEdgeView(PlutoManageAPIView):  # pylint: disable=abstract-method
    """API view for adding an edge to a node."""

    def handle_put(self, request: Request, **kwargs) -> Response:  # pylint: disable=unused-argument
        """Adds an edge to the node with the given name.

        Args:
            request: The incoming HTTP request (unused).
            **kwargs: URL parameters, must contain 'name'.

        Returns:
            A Response containing the serialized AddEdgeResponse.
            Returns HTTP 503 if the edge could not be added.
            Returns HTTP 500 if an unexpected error occurs.
        """
        name: str = kwargs["name"]
        try:
            edge: str = settings.PLUTO_EDGE_NAME
            if not DomainNode(name, self._logger).add_edge(edge):
                return Response(
                    status=HTTPStatus.SERVICE_UNAVAILABLE,
                    data={"result": False, "description": "Failed to add edge."},
                )
            return Response(
                status=HTTPStatus.OK,
                data={"result": True, "description": "Edge added."},
            )
        except Exception as e:  # pylint: disable=broad-exception-caught
            self._logger.exception(e)
            return Response(status=HTTPStatus.INTERNAL_SERVER_ERROR)

    pass


# ----------------------------------------------------------------------------------------------------------------------
