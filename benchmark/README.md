# Benchmark

Lasttests für die Pluto HTTP Interface Anwendung zur Messung von Latenz und Durchsatz.

## Voraussetzungen

```bash
pip install -r requirements.txt
```

## Szenario: Event-Roundtrip

Misst die End-to-End-Latenz vom Absenden eines POST-Requests bis zum Eintreffen
der korrespondierenden Antwort über den SSE-Eventstream.

```
POST /runtime/events/  →  SSE /runtime/events/
         t0                        t1
         └──────── Latenz ─────────┘
```

## Lasttest ausführen

```bash
locust -f scenarios/event_roundtrip.py \
  --host http://localhost:8000 \
  --headless \
  --users 10 \
  --spawn-rate 1 \
  --run-time 60s \
  --csv results/event_roundtrip
```

## Parameter

| Parameter | Beschreibung |
|---|---|
| `--users` | Anzahl gleichzeitiger Benutzer |
| `--spawn-rate` | Benutzer pro Sekunde die gestartet werden |
| `--run-time` | Laufzeit des Tests (z.B. `60s`, `5m`) |
| `--csv` | Präfix für die CSV-Ausgabedateien |

Zieladresse, Ziel-Knoten und Timeout können in `config/settings.py` angepasst werden.

## Ergebnisse

Die Ergebnisse werden im Ordner `results/` gespeichert:

| Datei | Inhalt |
|---|---|
| `*_stats.csv` | Latenz, RPS und Fehlerrate |
| `*_failures.csv` | Fehlgeschlagene Requests |
| `*_stats_history.csv` | Zeitverlauf der Metriken |
