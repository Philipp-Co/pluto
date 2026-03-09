
import fcntl
from os import environ, SEEK_END
from pathlib import Path
from base64 import b64encode
from json import dumps


class PlutoNode:

    MAX_FILE_SIZE_IN_BYTES: int = 25000000 # 25 mb

    def __init__(self):
        self.__file: str = ''
        pass

    def file(self) -> Path:
        return Path(
            '/pluto/nodes/http_edge/queue'
        )

    def setupDirectory(self) -> None:
        Path('/pluto/nodes/http_edge/').mkdir(parents=True, exist_ok=True)
        self.file().touch(exist_ok=True)
        pass

    def write(self, data: bytes) -> None:
        path: Path = self.file()
        with open(path, 'ab+') as file:
            try:
                fcntl.flock(file, fcntl.LOCK_EX)
                current_position: int = file.tell()
                size: int = file.seek(0, SEEK_END)
                file.seek(current_position)

                encoded_data: bytes = data + b'\n'
                if (size + len(encoded_data) + 1) > self.MAX_FILE_SIZE_IN_BYTES:
                    print('Queue full!')
                else:
                    file.write(encoded_data)
            except OSError:
                pass
            finally:
                fcntl.flock(file, fcntl.LOCK_UN)
        pass

    def setup(self, *args, **kwargs):
        self.setupDirectory()
        pass

    def teardown(self, *args, **kwargs):
        pass

    def run(self, id: int, event: int, number_of_output_queues: int, payload: str) -> str:
        print(
            f'Run: {type(payload)}'
        )
        if isinstance(payload, str):
            print(f'Write to Queuefile: {payload}')
            self.write(
                dumps(
                    {
                        'id': id,
                        'event_id': event,
                        'timestamp': '1900-01-01T00:00:00.0+00:00',
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
                        'timestamp': '1900-01-01T00:00:00.0+00:00',
                        'payload': payload.decode(),
                    }
                ).encode()
            )
        else:
            print('Unbekanntes Format der Eingabe!')
        return (0, 0, 0x0, ''.encode())

    pass

