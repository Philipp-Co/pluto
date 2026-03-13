from logging import getLogger, Logger, StreamHandler, Formatter
import fcntl
from os import environ, SEEK_END
from pathlib import Path
from base64 import b64encode
from json import dumps
from time import monotonic
from datetime import datetime, timezone
from sys import stdout


class PlutoNode:

    MAX_FILE_SIZE_IN_BYTES: int = 25000000 # 25 mb

    def __init__(self):
        self.__file: str = ''
        self.__logger: Logger = getLogger(self.__class__.__name__)
        self.__logger.setLevel('INFO')
        handler: StreamHandler = StreamHandler()
        formatter: Formatter = Formatter(
            "%(asctime)s | %(levelname)-8s | %(name)-18s | %(message)s"
        )
        handler.setFormatter(formatter)
        self.__logger.addHandler(handler)
        pass

    def file(self) -> Path:
        return Path(
            '/pluto/nodes/http_edge/queue'
        )

    def setupDirectory(self) -> None:
        Path('/pluto/nodes/http_edge/').mkdir(parents=True, exist_ok=True)
        self.file().touch(exist_ok=True)
        pass

    def write(self, data: bytes, timeout_in_seconds: float=1.0) -> None:
        path: Path = self.file()
        with open(path, 'ab+') as file:
            start: float = monotonic()
            while((monotonic() - start) < timeout_in_seconds):
                try:
                    fcntl.flock(file, fcntl.LOCK_EX)
                    current_position: int = file.tell()
                    size: int = file.seek(0, SEEK_END)
                    file.seek(current_position)

                    encoded_data: bytes = data + b'\n'
                    if (size + len(encoded_data) + 1) > self.MAX_FILE_SIZE_IN_BYTES:
                        self.__logger.warning('Queue full!')
                    else:
                        file.write(encoded_data)
                        break
                except OSError as e:
                    getLogger(self.__class__.__name__).exception(e)
                    pass
            fcntl.flock(file, fcntl.LOCK_UN)
        pass

    def setup(self, *args, **kwargs):
        self.__logger.info(
            'Hello World!'
        )
        self.setupDirectory()
        pass

    def teardown(self, *args, **kwargs):
        self.__logger.info(
            'Bye Bye.'
        )
        pass

    def run(self, id: int, event: int, number_of_output_queues: int, payload: bytes) -> str:
        start: float = monotonic()
        try:
            dt: datetime = datetime.now(tz=timezone.utc)
            tz: str = dt.strftime("%z")
            formatted: str = (
                f"{dt.strftime('%Y-%m-%dT%H:%M:%S')}"
                f".{dt.microsecond // 100000}"  # erste Stelle der Mikrosekunden
                f"{tz[:3]}:{tz[3:]}"           # '+0000' → '+00:00'
            )
            if isinstance(payload, str):
                self.write(
                    dumps(
                        {
                            'id': id,
                            'event_id': event,
                            'timestamp': formatted,
                            'payload': payload,
                        }
                    ).encode()
                )
            elif isinstance(payload, bytes):
                self.write(
                    dumps(
                        {
                            'id': id,
                            'event_id': event,
                            'timestamp': formatted,
                            'payload': payload.decode(),
                        }
                    ).encode()
                )
            else:
                self.__logger.error(
                    f'Unknown Type of "payload": {type(payload)}'
                )
        except Exception as e:
            self.__logger.exception(e)
        end: float = monotonic()
        self.__logger.info(
            f'Time to write Event to file {end - start}s.'
        )
        return (0, 0, 0x0, b'')

    pass

