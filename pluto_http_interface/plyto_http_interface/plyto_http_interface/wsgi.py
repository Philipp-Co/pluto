"""WSGI config for plyto_http_interface project.

Exposes the WSGI callable as a module-level variable named application.
"""

# ----------------------------------------------------------------------------------------------------------------------

import os

from django.core.wsgi import get_wsgi_application

# ----------------------------------------------------------------------------------------------------------------------

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "plyto_http_interface.settings")

application = get_wsgi_application()
