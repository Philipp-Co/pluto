# Systemtests

## Voraussetzungen

Docker muss installiert und verfügbar sein.

## Einrichtung

1. Virtuelle Umgebung anlegen und aktivieren:
```bash
python -m venv .venv
source .venv/bin/activate
```

2. Abhängigkeiten installieren:
```bash
pip install -r requirements.txt
```

## Tests starten

1. Container bauen:
```bash
docker-compose build
```

2. Tests ausführen (startet und stoppt den Container automatisch):
```bash
python -m unittest discover -s tests/
```

Der Port der Anwendung ist standardmäßig `10000` und entspricht der Portweiterleitung
in `docker-compose.yml`. Er kann über die Umgebungsvariable `PLUTO_HTTP_PORT` überschrieben
werden:
```bash
PLUTO_HTTP_PORT=10000 python -m unittest discover -s tests/
```
