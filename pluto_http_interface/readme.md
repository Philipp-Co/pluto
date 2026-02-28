# HTTP Interface

Interface with Pluto via HTTP.

## Passthrough

PUT http://localhost:8080/passthrough/

## Python

PUT http://localhost:8080/python/

## Core

### Health-Check

http://localhost:8080/health-check/

### Configuration

GET http://localhost:8080/configuration/nodes/
GET http://localhost:8080/configuration/nodes/node_name:str/

### Status

GET http://localhost:8080/status/nodes/
GET http://localhost:8080/status/nodes/node_name:str/

### Send Events

POST http://localhost:8080/status/nodes/node_name:str/

