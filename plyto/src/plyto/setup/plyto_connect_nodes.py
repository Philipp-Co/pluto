from json import loads, dumps
from typing_extensions import Self
from sys import argv
from logging import getLogger, Logger, StreamHandler, INFO, Formatter
from argparse import ArgumentParser


class ConnectNodes:

    def __init__(self, logger: Logger):
        self.__logger: Logger = logger
        pass

    def core_configuration_file(self) -> str:
        return '/pluto/core/config/core.cfg'
    
    def node_configuration_file(self, name: str) -> str:
        return f'/pluto/nodes/{name}/config/{name}.cfg'

    def node_input_queue_name(self, node_name: str) -> str:
        return f'{node_name}_iq'

    def connect(self, src: str, dest: str) -> Self:
        src_config = self.node_configuration_file(src)

        content = {}
        with open(src_config, "r") as file:
            content = loads(file.read())
        content['names_of_output_queues'].append(
            self.node_input_queue_name(dest) 
        )
        with open(src_config, "w") as file:
            file.write(
                f'{dumps(content)}\n'
            )
        return self
    
    def disconnect(self, src: str, dest: str) -> Self:
        src_config = self.node_configuration_file(src)
        with open(src_config, "r") as file:
            content = loads(file.read())
        if content['names_of_output_queues'].index(self.node_input_queue_name(dest)) < 0:
            return self
        content['names_of_output_queues'].remove(
            self.node_input_queue_name(dest) 
        )
        with open(src_config, "w") as file:
            file.write(
                f'{dumps(content)}\n'
            )
        return self

    pass


def cli():

    arg_parser: ArgumentParser = ArgumentParser()
    arg_parser.add_argument(
        '-s',
        '--source',
        nargs=1,
    )
    arg_parser.add_argument(
        '-t',
        '--target',
        nargs=1,
    )
    arg_parser.add_argument(
        '-a',
        '--add',
        action='store_true',
    )
    arg_parser.add_argument(
        '-r',
        '--remove',
        action='store_true'
    )
    args = arg_parser.parse_args(argv[1: len(argv)])

    logger: Logger = getLogger(__name__)
    stream_handler: StreamHandler = StreamHandler()
    stream_handler.setLevel(INFO)
    stream_handler.setFormatter(
        Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    )
    logger.addHandler(stream_handler)

    if args.add and args.remove:
        logger.info(
            'Add and Remove are exclusive tasks!'
        )
        return -1

    conn: ConnectNodes = ConnectNodes(
        logger
    )
    if args.add:
        conn.connect(args.source[0], args.target[0])
        return 0
    elif args.remove:
        conn.disconnect(args.source[0], args.target[0])
        return 0
    return -1
