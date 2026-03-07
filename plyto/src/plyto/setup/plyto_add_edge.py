from json import loads, dumps
from typing_extensions import Self
from sys import argv
from logging import getLogger, Logger, StreamHandler, INFO, Formatter


class AddEdge:

    def __init__(self, logger: Logger):
        self.__logger: Logger = logger
        pass

    def core_configuration_file(self) -> str:
        return '/pluto/core/config/core.cfg'
    
    def node_configuration_file(self, name: str) -> str:
        return f'/pluto/nodes/{name}/config/{name}.cfg'

    def connect(self, src: str, dest: str) -> Self:
        """Connect the Node src with the Queue dest."""
        src_config = self.node_configuration_file(src)

        content = {}
        with open(src_config, "r") as file:
            content = loads(file.read())
        content['names_of_output_queues'].append(
            dest
        )
        with open(src_config, "w") as file:
            file.write(
                f'{dumps(content)}\n'
            )
        return self

    pass


def cli():
    logger: Logger = getLogger(__name__)
    stream_handler: StreamHandler = StreamHandler()
    stream_handler.setLevel(INFO)
    stream_handler.setFormatter(
        Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    )
    logger.addHandler(stream_handler)
    
    conn: AddEdge = AddEdge(
        logger
    )
    conn.connect(argv[1], argv[2])
