# Pluto Systemtest

## Arbeitsweise
Vor jeder Implementierung wird zunächst ein Vorschlag unterbreitet und auf Bestätigung gewartet.

## Ziel
Systemtests für Pluto. Die Anwendung wird als Blackbox getestet – ausschließlich über die nach außen sichtbaren HTTP-Schnittstellen. Interne Implementierungsdetails sind nicht Gegenstand dieser Tests.

## Technologien
- Python 3
- unittest (Standardbibliothek)
- requests (HTTP-Client)
- Docker / Docker Compose (Testumgebung)

## Testumgebung
Das Pluto Docker Image wird über `docker-compose.yml` gebaut und gestartet. Jeder Test-Case fährt die Umgebung frisch hoch (`setUp`) und reißt sie nach dem Test wieder ab (`tearDown`). Der Health-Check des Containers wird abgewartet, bevor die Tests starten.

Standardport: `10000` (konfigurierbar über die Umgebungsvariable `PLUTO_HTTP_PORT`).

Tests starten mit:
```bash
python -m unittest discover -s tests/
```

## Architektur
- `tests/utils/` – Basisklasse `PlutoSystemtestBase` und gemeinsame Konstanten
- `tests/management/` – Tests für Management-Endpunkte
- `tests/runtime/` – Tests für Runtime-Endpunkte

## Qualität
- Typisierung mit Python Type Hints
- Es wird nach dem Google Python Style Guide gearbeitet
- Jede Datei, Klasse und Funktion erhält einen Docstring
- Zwischen dem Datei-Docstring und den Imports wird eine Trennlinie eingefügt
- Zwischen den Imports und dem restlichen Code wird eine Trennlinie eingefügt
- Klassen werden mit einer Trennlinie abgeschlossen
