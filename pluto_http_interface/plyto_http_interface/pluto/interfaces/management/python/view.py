"""Base view class for Pluto management API views."""

# ----------------------------------------------------------------------------------------------------------------------

from http import HTTPStatus
from logging import Logger, getLogger

from rest_framework.request import Request
from rest_framework.response import Response
from rest_framework.views import APIView

from pluto.domain.manage.core import Core, CoreState

# ----------------------------------------------------------------------------------------------------------------------


class PlutoManageAPIView(APIView):
    """Base class for Pluto management views.

    Provides logger initialization and the Template Method pattern for
    put and delete HTTP handlers, including Core state guard.
    """

    def __init__(self, **kwargs) -> None:
        """Initializes the view with a class-named logger."""
        super().__init__(**kwargs)
        self._logger: Logger = getLogger(self.__class__.__name__)
        pass

    def put(self, request: Request, **kwargs) -> Response:
        """Checks Core state and delegates to handle_put."""
        if Core(self._logger).state() != CoreState.INITIAL:
            return Response(
                status=HTTPStatus.CONFLICT,
                data={"result": False, "description": "Core is not stopped."},
            )
        return self.handle_put(request, **kwargs)

    def delete(self, request: Request, **kwargs) -> Response:
        """Checks Core state and delegates to handle_delete."""
        if Core(self._logger).state() != CoreState.INITIAL:
            return Response(
                status=HTTPStatus.CONFLICT,
                data={"result": False, "description": "Core is not stopped."},
            )
        return self.handle_delete(request, **kwargs)

    def handle_put(self, request: Request, **kwargs) -> Response:
        """Handles a PUT request after the Core state has been verified."""
        raise NotImplementedError

    def handle_delete(self, request: Request, **kwargs) -> Response:
        """Handles a DELETE request after the Core state has been verified."""
        raise NotImplementedError

    pass


# ----------------------------------------------------------------------------------------------------------------------
