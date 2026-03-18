"""Konfigurationseinstellungen für das Benchmark-Projekt.

Enthält Zieladressen, Timeouts sowie Last-Parameter wie
Benutzeranzahl und Spawn-Rate für die Lasttests gegen
die Pluto HTTP Interface Anwendung.
"""

# ----------------------------------------------------------------------------------------------------------------------

HOST: str = "http://localhost:10000"
"""Zieladresse der Pluto HTTP Interface Anwendung."""

TARGET_NODE_NAME: str = "test"
"""Name des Ziel-Knotens für die Event-Roundtrip-Tests."""

REQUEST_TIMEOUT: float = 120.0
"""Maximale Wartezeit in Sekunden auf ein SSE-Event."""

REQUESTS_PER_TASK: int = 50
"""Anzahl der POST-Requests pro Task-Ausführung."""
