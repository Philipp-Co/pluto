"""HTTP interface for managing nodes and uploading node packages.

Provides AddNodeView for creating and deleting nodes by name, and
UploadArchive for uploading a node package archive. Also defines the
serializers used for request deserialization and response serialization.
"""

# ----------------------------------------------------------------------------------------------------------------------------------------------

from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework.request import Request
from rest_framework.serializers import Serializer, CharField, BooleanField
from drf_spectacular.utils import extend_schema
from http import HTTPStatus
from logging import Logger, getLogger
from json import loads, JSONDecodeError
from base64 import b64decode
from pluto.domain.manage.core import Core, CoreState
from pluto.domain.manage.node import Node as DomainNode


# ----------------------------------------------------------------------------------------------------------------------------------------------


class ManageAddNodeRequestSerializer(Serializer):
    """Deserializes an add-node request.

    Fields:
        top_level_package_name: The top-level Python package name of the node (max 64 characters).
    """

    top_level_package_name = CharField(max_length=64)

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


class ManageNodeSerializer(Serializer):
    """Deserializes a node management request.

    Fields:
        (none required — the node name is provided via the URL parameter)
    """

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


class ManageResponseSerializer(Serializer):
    """Serializes the result of a node management operation.

    Fields:
        result: Indicates whether the operation was successful.
        description: Human-readable description of the outcome.
    """

    result      = BooleanField()
    description = CharField(max_length=64)

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


class UploadArchiveRequestSerializer(Serializer):
    """Deserializes an upload-archive request.

    Fields:
        content: The base64-encoded binary content of the package archive.
    """

    content = CharField()

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


class AddNodeView(APIView):
    """API view for creating and deleting nodes by name."""

    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)
        self.__logger: Logger = getLogger(self.__class__.__name__)
        pass

    @extend_schema(description='Creates a node with the given name. Requires the core process to be in the STOPPED state and a package archive to be present for the given node name. The core process can be stopped via the runtime control HTTP endpoint.', request=ManageAddNodeRequestSerializer, responses={HTTPStatus.OK.value: ManageResponseSerializer, HTTPStatus.CONFLICT.value: ManageResponseSerializer, HTTPStatus.BAD_REQUEST.value: None, HTTPStatus.NOT_FOUND.value: ManageResponseSerializer, HTTPStatus.SERVICE_UNAVAILABLE.value: ManageResponseSerializer, HTTPStatus.INTERNAL_SERVER_ERROR.value: None})
    def put(self, request: Request, name: str) -> Response:
        """Creates a node with the given name.

        Requires the core process to be in the STOPPED state and a package
        archive to be present for the given node name. The core process can be
        stopped via the RuntimeControlView.

        Args:
            request: The HTTP request containing a JSON body with top_level_package_name.
            name: The name of the node to create, provided via the URL parameter.

        Returns:
            A Response containing the serialized ManageResponse.
            Returns HTTP 409 if the core process is not in the STOPPED state.
            Returns HTTP 400 if the request body is missing or invalid.
            Returns HTTP 404 if no package archive exists for the given node name.
            Returns HTTP 503 if the node could not be updated.
            Returns HTTP 500 if an unexpected error occurs.
        """
        try:
            if Core(self.__logger).state() != CoreState.INITIAL:
                return Response(
                    status=HTTPStatus.CONFLICT,
                    data={
                        'result': False,
                        'description': 'Core is not stopped.',
                    }
                )
            if request.body is None:
                return Response(status=HTTPStatus.BAD_REQUEST)
            payload: str = request.body.decode()
            try:
                serializer: ManageAddNodeRequestSerializer = ManageAddNodeRequestSerializer(data=loads(payload))
                if not serializer.is_valid():
                    self.__logger.warning(payload)
                    return Response(status=HTTPStatus.BAD_REQUEST)
            except JSONDecodeError:
                self.__logger.warning(payload)
                return Response(status=HTTPStatus.BAD_REQUEST)
            if DomainNode(name, self.__logger).node_exists():
                return Response(
                    status=HTTPStatus.OK,
                    data={
                        'result': True,
                        'description': 'Node created.',
                    }
                )
            if not DomainNode(name, self.__logger).archive_exists():
                return Response(
                    status=HTTPStatus.NOT_FOUND,
                    data={
                        'result': False,
                        'description': 'Archive not found.',
                    }
                )
            if not DomainNode(name, self.__logger).create_node(top_level_package_name=serializer.validated_data['top_level_package_name']):
                return Response(
                    status=HTTPStatus.SERVICE_UNAVAILABLE,
                    data={
                        'result': False,
                        'description': 'Failed to update node.',
                    }
                )
            return Response(
                status=HTTPStatus.OK,
                data={
                    'result': True,
                    'description': 'Node created.',
                }
            )
        except Exception as e:
            self.__logger.exception(e)
            return Response(status=HTTPStatus.INTERNAL_SERVER_ERROR)

    @extend_schema(description='Deletes the node with the given name. Requires the core process to be in the STOPPED state. The core process can be stopped via the runtime control HTTP endpoint.', request=None, responses={HTTPStatus.OK.value: ManageResponseSerializer, HTTPStatus.CONFLICT.value: ManageResponseSerializer, HTTPStatus.INTERNAL_SERVER_ERROR.value: None})
    def delete(self, request: Request, name: str) -> Response:
        """Deletes the node with the given name.

        Requires the core process to be in the STOPPED state. The core process
        can be stopped via the RuntimeControlView.

        Args:
            request: The incoming HTTP request.
            name: The name of the node to delete, provided via the URL parameter.

        Returns:
            A Response containing the serialized ManageResponse.
            Returns HTTP 409 if the core process is not in the STOPPED state.
            Returns HTTP 500 if an unexpected error occurs.
        """
        try:
            if Core(self.__logger).state() != CoreState.INITIAL:
                return Response(
                    status=HTTPStatus.CONFLICT,
                    data={
                        'result': False,
                        'description': 'Core is not stopped.',
                    }
                )
            DomainNode(name, self.__logger).archive_delete()
            return Response(
                {
                    'result': True,
                    'description': 'Node deleted.',
                }
            )
        except Exception as e:
            self.__logger.exception(e)
            return Response(status=HTTPStatus.INTERNAL_SERVER_ERROR)

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------


class UploadArchive(APIView):
    """API view for uploading a node package archive."""

    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)
        self.__logger: Logger = getLogger(self.__class__.__name__)
        pass

    @extend_schema(description='Uploads a package archive for the node with the given name. The archive must have the file extension .tar.gz. Requires the core process to be in the STOPPED state. The core process can be stopped via the runtime control HTTP endpoint.', request=UploadArchiveRequestSerializer, responses={HTTPStatus.OK.value: ManageResponseSerializer, HTTPStatus.BAD_REQUEST.value: ManageResponseSerializer, HTTPStatus.CONFLICT.value: ManageResponseSerializer, HTTPStatus.SERVICE_UNAVAILABLE.value: ManageResponseSerializer, HTTPStatus.INTERNAL_SERVER_ERROR.value: None})
    def put(self, request: Request, name: str) -> Response:
        """Uploads a package archive for the node with the given name.

        Args:
            request: The HTTP request containing a JSON body with the base64-encoded archive content.
            name: The name of the node, provided via the URL parameter.

        Returns:
            A Response containing the serialized ManageResponse with HTTP 200.
            Returns HTTP 400 if the request body is missing or invalid.
            Returns HTTP 409 if the core process is not in the STOPPED state.
            Returns HTTP 503 if writing the archive file failed.
            Returns HTTP 500 if an unexpected error occurs.
        """
        try:
            if request.body is None:
                return Response(
                    status=HTTPStatus.BAD_REQUEST,
                    data={
                        'result': False,
                        'description': 'Missing request body.',
                    }
                )
            payload: str = request.body.decode()
            try:
                serializer: UploadArchiveRequestSerializer = UploadArchiveRequestSerializer(data=loads(payload))
                if not serializer.is_valid():
                    self.__logger.warning(payload)
                    return Response(
                        status=HTTPStatus.BAD_REQUEST,
                        data={
                            'result': False,
                            'description': 'Invalid request body.',
                        }
                    )
            except JSONDecodeError:
                self.__logger.warning(payload)
                return Response(
                    status=HTTPStatus.BAD_REQUEST,
                    data={
                        'result': False,
                        'description': 'Invalid request body.',
                    }
                )
            if Core(self.__logger).state() != CoreState.INITIAL:
                return Response(
                    status=HTTPStatus.CONFLICT,
                    data={
                        'result': False,
                        'description': 'Core is not stopped.',
                    }
                )
            content: bytes = b64decode(serializer.validated_data['content'])
            if not DomainNode(name, self.__logger).archive_write(content):
                return Response(
                    status=HTTPStatus.SERVICE_UNAVAILABLE,
                    data={
                        'result': False,
                        'description': 'Failed to write archive.',
                    }
                )
            return Response(
                status=HTTPStatus.OK,
                data={
                    'result': True,
                    'description': 'Ok.',
                }
            )
        except Exception as e:
            self.__logger.exception(e)
            return Response(status=HTTPStatus.INTERNAL_SERVER_ERROR)

    pass


# ----------------------------------------------------------------------------------------------------------------------------------------------
