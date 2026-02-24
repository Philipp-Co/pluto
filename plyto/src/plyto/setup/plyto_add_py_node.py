
from plyto.setup.nodes.py_node import PythonNode
from sys import argv, stdout
from logging import getLogger, Logger, StreamHandler, INFO, Formatter


def cli():
    logger: Logger = getLogger(__name__)
    logger.setLevel(INFO)
    stream_handler: StreamHandler = StreamHandler(stream=stdout)
    stream_handler.setFormatter(
        Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    )
    logger.addHandler(stream_handler)

    node: PythonNode = PythonNode(
        logger=logger,
        name=argv[1],
        path_isntallable_package=argv[2],
        package_name=argv[3],
    )
    node.create_empty_config()
    node.create_venv()
    node.install()

