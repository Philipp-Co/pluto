"""URL configuration for the Pluto HTTP interface."""

# ----------------------------------------------------------------------------------------------------------------------

from django.urls import include, path
from drf_spectacular.views import SpectacularAPIView

from pluto.interfaces.management.python.add_edge import AddEdgeView
from pluto.interfaces.management.python.add_node import AddNodeView, UploadArchive
from pluto.interfaces.management.python.connect_node import ConnectNodeView
from pluto.interfaces.management.python.structure import NodeStructureView
from pluto.interfaces.management.runtime import RuntimeControlView
from pluto.interfaces.runtime.node import RuntimeNodeView

# ----------------------------------------------------------------------------------------------------------------------

urlpatterns = [
    path("manage/node/add/<str:name>/", AddNodeView.as_view()),
    path("manage/node/add/<str:name>/package/", UploadArchive.as_view()),
    path("manage/node/", NodeStructureView.as_view()),
    path("manage/node/<str:name>/edge/", AddEdgeView.as_view()),
    path("manage/node/connect/", ConnectNodeView.as_view()),
    path("manage/", RuntimeControlView.as_view()),
    path("runtime/events/", RuntimeNodeView.as_view()),
    path("schema/", SpectacularAPIView.as_view(), name="schema"),
    path("health/", include("health_check.urls")),
]
