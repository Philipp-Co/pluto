"""Gemeinsame Basisklassen für alle Lasttest-Szenarien.

Stellt eine gemeinsame Basisklasse bereit, von der alle Szenarien
erben können.
"""

# ----------------------------------------------------------------------------------------------------------------------

from locust import HttpUser

# ----------------------------------------------------------------------------------------------------------------------


class PlutoBaseUser(HttpUser):
    """Basis-Locust-User für die Pluto HTTP Interface Lasttests."""

    abstract = True

    # ------------------------------------------------------------------------------------------------------------------
