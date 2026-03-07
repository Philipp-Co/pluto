"""HTTP interface for querying the node structure."""

# ----------------------------------------------------------------------------------------------------------------------

from http import HTTPStatus
from logging import Logger, getLogger

from drf_spectacular.utils import extend_schema
from rest_framework.request import Request
from rest_framework.response import Response
from rest_framework.serializers import BooleanField, CharField, ListField, Serializer
from rest_framework.views import APIView

from pluto.domain.manage.structure import NodeStructure as DomainNodeStructure

# ----------------------------------------------------------------------------------------------------------------------


class NodeConnectionSerializer(Serializer):  # pylint: disable=abstract-method
    """Serializes a connection between two nodes.

    Fields:
        source: The name of the source node.
        target: The name of the target node.
    """

    source = CharField()  # type: ignore[assignment]
    target = CharField()  # type: ignore[assignment]

    pass


# ----------------------------------------------------------------------------------------------------------------------


class NodeStructureSerializer(Serializer):  # pylint: disable=abstract-method
    """Serializes the current node structure.

    Fields:
        result: Indicates whether the operation was successful.
        description: Human-readable description of the outcome.
        nodes: Names of all defined nodes.
        connections: List of connections between nodes.
    """

    result = BooleanField()
    description = CharField()
    nodes = ListField(child=CharField())
    connections = NodeConnectionSerializer(many=True)

    pass


# ----------------------------------------------------------------------------------------------------------------------


class NodeStructureView(APIView):
    """API view for querying the current node structure."""

    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)
        self.__logger: Logger = getLogger(self.__class__.__name__)
        pass

    @extend_schema(
        description="Returns the current node structure including all defined nodes and their connections.",
        request=None,
        responses={
            HTTPStatus.OK.value: NodeStructureSerializer,
            HTTPStatus.SERVICE_UNAVAILABLE.value: NodeStructureSerializer,
            HTTPStatus.INTERNAL_SERVER_ERROR.value: NodeStructureSerializer,
        },
    )
    def get(self, _request: Request) -> Response:
        """Returns the current node structure.

        Args:
            request: The incoming HTTP request.

        Returns:
            A Response containing the serialized NodeStructure.
            Returns HTTP 503 if the structure could not be retrieved.
            Returns HTTP 500 if an unexpected error occurs.
        """
        try:
            structure = DomainNodeStructure(self.__logger).get()
            if structure is None:
                return Response(
                    status=HTTPStatus.SERVICE_UNAVAILABLE,
                    data=NodeStructureSerializer(
                        {
                            "result": False,
                            "description": "Failed to retrieve node structure.",
                            "nodes": [],
                            "connections": [],
                        }
                    ).data,
                )
            return Response(
                status=HTTPStatus.OK,
                data=NodeStructureSerializer(
                    {
                        "result": True,
                        "description": "Ok.",
                        "nodes": structure.nodes,
                        "connections": [{"source": s, "target": t} for s, t in structure.connections],
                    }
                ).data,
            )
        except Exception as e:  # pylint: disable=broad-exception-caught
            self.__logger.exception(e)
            return Response(
                status=HTTPStatus.INTERNAL_SERVER_ERROR,
                data=NodeStructureSerializer(
                    {
                        "result": False,
                        "description": "Internal server error.",
                        "nodes": [],
                        "connections": [],
                    }
                ).data,
            )

    pass


# ----------------------------------------------------------------------------------------------------------------------
