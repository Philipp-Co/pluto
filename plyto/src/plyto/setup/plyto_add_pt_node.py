
from plyto.setup.nodes.pt_node import PassthroughNode
from sys import argv
from logging import getLogger, Logger, StreamHandler, INFO, Formatter


def cli():
    logger: Logger = getLogger(__name__)
    stream_handler: StreamHandler = StreamHandler()
    stream_handler.setLevel(INFO)
    stream_handler.setFormatter(
        Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    )
    logger.addHandler(stream_handler)

    node: PassthroughNode = PassthroughNode(
        logger=logger,
        name=argv[1],
    )
    node.create_empty_config()

