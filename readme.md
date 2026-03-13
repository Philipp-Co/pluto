# Pluto

Pluto is an event-based framework for developing event-driven applications on a single device instance.

## Directory Overview

### benchmark/
Load tests for the HTTP interface based on Locust.

### bin/
Shell scripts for managing the Pluto system (start/stop nodes, connect, disconnect, send events).

### demo/
Example application demonstrating the framework.

### docker/
Docker configuration files for running Pluto in containers.

### external/
External dependencies: `jsmn` (JSON parser) and `unity` (unit testing framework).

### pluto_config/
Configuration component of the framework.

### pluto_core/
Core component of the framework.

### pluto_edge/
C API for integrating existing applications into the Pluto infrastructure.

### pluto_http_interface/
HTTP interface for Pluto based on Django REST Framework and ASGI.

### pluto_internal_nodes/
Internal Pluto nodes, e.g. the HTTP edge node.

### pluto_node/
Central node application. Manages the IPC infrastructure and provides interfaces for business logic.

### pluto_os_abstraction/
OS abstraction layer of the framework.

### plyto/
Python package for interacting with Pluto.

### systemtest/
System tests for the HTTP interface (Python/unittest, black-box tests over HTTP).
