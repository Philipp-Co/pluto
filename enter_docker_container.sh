#!/bin/bash
docker exec -it $(docker ps | tail -n 1 | cut -c1-12) /bin/bash
