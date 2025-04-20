# Docker

## build-args

### PYTHON_PATH

PYTHON_PATH: Eine durch ";" getrennte Liste aus Pfaden die dem Python Path des eingebetteten Interpreters hinzugefuegt werden.

    PYTHON_PATH=/user/pluto/home/project/;/bin/

## Das Image bauen

    docker build --build-arg PYTHON_PATH=... -t pluto_base -f dockerfile ../

