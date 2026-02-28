from django.urls import path
from drf_spectacular.views import SpectacularAPIView
from pluto.interfaces.management.python.add_node import AddNodeView, UploadArchive
from pluto.interfaces.management.python.connect_node import ConnectNodeView
from pluto.interfaces.management.runtime import RuntimeControlView
from pluto.interfaces.runtime.node import RuntimeNodeView

urlpatterns = [
    path('manage/node/add/<str:name>/', AddNodeView.as_view()),
    path('manage/node/add/<str:name>/package/', UploadArchive.as_view()),
    path('manage/node/connect/', ConnectNodeView.as_view()),
    path('manage/', RuntimeControlView.as_view()),
    path('runtime/events/', RuntimeNodeView.as_view()),
    path('schema/', SpectacularAPIView.as_view(), name='schema'),
]
