from os import environ

# Port of the Pluto HTTP interface on the host machine.
# Override with the PLUTO_HTTP_PORT environment variable if needed.
# Default matches the port mapping defined in docker-compose.yml.
PLUTO_HTTP_PORT: int = int(environ.get('PLUTO_HTTP_PORT', '10000'))

BASE_URL: str = f'http://localhost:{PLUTO_HTTP_PORT}'
