# CLAUDE.md – Plyto Client

## Wichtig: Planung vor Implementierung

**Vor jeder Implementierung muss ein Vorschlag gemacht werden.**
Kein Code wird geschrieben, bevor der Nutzer den Plan explizit genehmigt hat.
Claude Code verwendet dafür immer den Plan Mode.

## Projekt

- **Sprache:** Python 3.8+
- **Build:** `setuptools` via `pyproject.toml`
- **Paketname:** `new_horizon`
- **Beschreibung:** Client-Bibliothek für die Interaktion mit Pluto (Event-driven Framework)

## Setup

```bash
python -m venv .venv
source .venv/bin/activate && pip install -r requirements.txt
```

## Tests ausführen

```bash
python -m unittest discover test
```

## Bauen

```bash
python -m build --sdist
```

## Umgebungsvariablen

| Variable          | Beschreibung                     |
|-------------------|----------------------------------|
| `PLUTO_WORKDIR`   | Arbeitsverzeichnis für Pluto     |
| `PLUTO_BINARYDIR` | Pfad zu Pluto-Binaries/Libraries |

## CLI Entry Points

- `plyto_admin` → `new_horizon.cli.plyto_admin_cli:cli`
- `plyto_event` → `new_horizon.cli.plyto_event_cli:cli`
