# Pluto

Pluto ist ein Eventzentriertes Framework.
Pluto setzt die Infrastruktur fuer die Entwicklung einer Eventgetriebenen Anwendung auf einer Geraeteinstanz um. 

## Was stellt Pluto zur Verfuegung?

Pluto ist in verschiedene Komponenten zerlegt, die entsprechend in die Entwicklung einer Anwendung eingebunden werden koennen.

### pluto_node

pluto_node ist die zentrale Komponente des Frameworks. pluto_node ist eine Anwendung die die von ihre verwendete Infrastruktur fuer die notwendige inter Prozess Kommunikation erstellt, verwaltet und verwendet.
pluto_node definiert mehrere Schnittstellen ueber die es moeglich ist Fachlogik zu integrieren.
Die Fachlogik kann in den pluto_node Kontext durch das bereitstellen einer der beiden Moeglichkeiten

    - Pythonskript
    - Shared Library

injeziert werden.

### pluto_edge

Sollen bestehende Anwendungen in die Plutoinfrastruktur eingebunden werden, stellt Pluto pluto_edge bereit.
pluto_edge definiert eine C API zum Zugriff auf die von Pluto erstellte inter Prozess Kommunikations Infrastruktur.

## Projektaufbau

    +--------+--------+---------+
    |  Info  |  Node  |  Core   |
    +--------+--------+---------+
    |           Edge            |
    +-------------+-------------+
    |     Event   |    Config   |
    +-------------+-------------+
    |    OS Abstraction         |
    +---------------------------+

## Externe Abhaengigkeiten

Externe Abhaengigkeiten werden um Verzeichnis unter external/ gesammelt.

### jsmn

jsmn ist ein JSON Parser der von Pluto genutzt wird. Siehe dazu auch https://github.com/zserge/jsmn .

### unity

Dieses Projekt wird mit Hilfe von unity getestet. Siehe dazu https://github.com/ThrowTheSwitch/Unity.git .

## Das Projekt bauen

    mkdir build/ && cd build/
    cmake -G "Unix Makefiles" ../
    make

### Testflags

Fuer erweiterte Tests kann das Flag 

    PLUTO_TEST

definiert werden.
Folgendes Flag wird beim generieren hinzugefuegt:

    -DPLUTO_TEST=1

Zusaetzliche Testfunktionen beinhalten:
    - Pruefungen fuer das Speichermanagement

