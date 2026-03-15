# Pluto Internal Nodes

## Arbeitsweise
Vor jeder Implementierung wird zunächst ein Vorschlag unterbreitet und auf Bestätigung gewartet.

## Beschreibung
Dieses Verzeichnis enthält Implementierungen interner Nodes, die von der Pluto-Anwendung zur Laufzeit genutzt werden.

## Unterverzeichnisse
- `pluto_http_edge/` – HTTP-Edge-Node: Bindeglied zwischen der Pluto-IPC-Infrastruktur und der HTTP-Schnittstelle
- `examplenode/`     – Beispiel-Node zur Demonstration der Node-Implementierung

## Qualität
- Typisierung mit Python Type Hints
- Jede Datei, Klasse und Funktion erhält einen Docstring
- Es wird nach dem Google Python Style Guide gearbeitet
- Zwischen dem Datei-Docstring und den Imports wird eine Trennlinie eingefügt
- Zwischen den Imports und dem restlichen Code wird eine Trennlinie eingefügt
- Klassen werden mit einer Trennlinie abgeschlossen
