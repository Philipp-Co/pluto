# Pluto HTTP Interface

## Arbeitsweise
Vor jeder Implementierung wird zunächst ein Vorschlag unterbreitet und auf Bestätigung gewartet.

## Technologien
- Python 3
- Django / Django REST Framework
- drf-spectacular (OpenAPI)
- uvicorn (ASGI)

## Architektur
- `domain/` – Business Logic, entkoppelt von HTTP
- `interfaces/` – HTTP-Endpunkte (Views & Serializer)

## Qualität
- Typisierung mit Python Type Hints
- Logging über den Django-Logger
- Es wird nach dem Google Python Style Guide gearbeitet
- Jede Datei, Klasse und Funktion erhält einen Docstring
- Zwischen dem Datei-Docstring und den Imports wird eine Trennlinie eingefügt
- Zwischen den Imports und dem restlichen Code wird eine Trennlinie eingefügt
- Klassen werden mit einer Trennlinie abgeschlossen
