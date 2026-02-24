from json import loads, dumps
from typing_extensions import Self
from typing import Any
from sys import argv
from logging import getLogger, Logger, StreamHandler, INFO, Formatter
from argparse import ArgumentParser
from plyto.edge.plyto_edge import PlytoEdgeFactory, PlytoEdge, PlytoInputEdge, PlytoOutputEdge, PlytoCoreConfig

class Parser:
    
    def __init__(self):
        self.__parser: ArgumentParser = ArgumentParser()
        self.__parser.add_argument(
            '-r',
            '--read',
            action='store_true',
            help='Read an Event from a given Queue.',
        )
        self.__parser.add_argument(
            '-w',
            '--write',
            action='store_true',
            help='Write an Event to a given Queue.',
        )
        self.__parser.add_argument(
            '-n',
            '--name',
            nargs=1,
            help='Name of a Queue.'
        )
        self.__parser.add_argument(
            '-p',
            '--payload',
            nargs=1,
            default=None,
            help='Payload for a Event which is written to a Queue.'
        )
        self.__parser.add_argument(
            '-l',
            '--list',
            action='store_true',
            help='List all available Queues.'
        )
        pass

    def parse(self, argv) -> Any:
        return self.__parser.parse_args(argv)

    pass

class Edge:
    
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
    
    def send(self, dest: str, payload: str) -> Self:
        edge: PlytoEdge = PlytoEdgeFactory.as_input_to_node(dest)
        edge.send(payload, 0, 0)
        return self

    def recv(self, src: str) -> str:
        return ''

    pass


def cli():
    args = Parser().parse(argv[1:])
    if args.list:
        print(
            'Queues:\n'
            '  ...'
        )
        return 0
    
    if args.read and args.write:
        print(
            'Error: Unable to read and write at the same time...'
        )
        return -1

    if args.write and args.payload is None:
        print(
            'Error: Unable to write an Event without a given Payload.'
        )
        return -1
    
    logger: Logger = getLogger(__name__)
    logger.setLevel(INFO)
    stream_handler: StreamHandler = StreamHandler()
    stream_handler.setFormatter(
        Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    )
    logger.addHandler(stream_handler)

    if args.write:
        logger.info(
            f'Write "{args.payload[0]}" to Queue "{args.name[0]}".'
        )
        Edge(logger).send(args.name[0], args.payload[0])
        return 0
    
    if args.read:
        logger.info(
            f'Read ... from Queue "{args.name[0]}".'
        )
        logger.info(
            Edge(logger).recv(args.name[0])
        )
        return 0
    return -1
