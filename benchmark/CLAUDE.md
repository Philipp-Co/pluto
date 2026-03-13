# Benchmark

## Arbeitsweise
Vor jeder Implementierung wird zunächst ein Vorschlag unterbreitet und auf Bestätigung gewartet.

## Ziel
Messung von Latenz und Durchsatz der Pluto HTTP Interface Anwendung unter Last.

## Technologien
- Python 3
- locust (Lasttest-Framework)

## Architektur
- `scenarios/` – Lasttest-Szenarien
- `results/`   – Messergebnisse
- `reports/`   – Auswertungen

## Qualität
- Typisierung mit Python Type Hints
- Jede Datei, Klasse und Funktion erhält einen Docstring
- Es wird nach dem Google Python Style Guide gearbeitet
- Zwischen dem Datei-Docstring und den Imports wird eine Trennlinie eingefügt
- Zwischen den Imports und dem restlichen Code wird eine Trennlinie eingefügt
- Klassen werden mit einer Trennlinie abgeschlossen
