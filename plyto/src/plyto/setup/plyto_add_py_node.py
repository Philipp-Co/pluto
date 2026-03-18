
from plyto.setup.nodes.py_node import PythonNode
from sys import argv, stdout
from logging import getLogger, Logger, StreamHandler, INFO, Formatter
from sys import argv
from argparse import ArgumentParser


def cli():
    logger: Logger = getLogger(__name__)
    logger.setLevel(INFO)
    stream_handler: StreamHandler = StreamHandler(stream=stdout)
    stream_handler.setFormatter(
        Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    )
    logger.addHandler(stream_handler)

    parser: ArgumentParser = ArgumentParser()
    parser.add_argument(
        '-a',
        '--add',
        action='store_true',
    )
    parser.add_argument(
        '-r',
        '--remove',
        action='store_true',
    )
    parser.add_argument(
        '-n',
        '--name',
        nargs=1,
    )
    parser.add_argument(
        '-i',
        '--installable_package',
        nargs=1,
    )
    parser.add_argument(
        '-p',
        '--package_name',
        nargs=1,
    )
    parser.add_argument(
        '-u',
        '--user-arguments',
        default='',
        type=str,
    )

    args = parser.parse_args(argv[1: len(argv)])
    if args.add and args.remove:
        logger.error('Add and Remove are exclusive.')
        return -1

    if args.add:
        if len(args.name) != 1 and len(args.installable_package) != 1 and len(args.package_name) != 1:
            logger.error(
                'If you want to add a new Node you must pass a Name, installable Package and a Packagename.'
            )
            return -1

        node: PythonNode = PythonNode(
            logger=logger,
            name=args.name[0],
            path_isntallable_package=args.installable_package[0],
            package_name=args.package_name[0],
        )
        node.create_empty_config(user_arguments=args.user_arguments)
        node.create_venv()
        node.install()
        return 0
    elif args.remove:
        if len(args.name) != 1:
            logger.error(
                'You must provide a Name to remove a Node.'
            )
            return -1
        node: PythonNode = PythonNode(
            logger=logger,
            name=args.name[0],
            path_isntallable_package='',#args.installable_package[0],
            package_name='',#args.package_name[0],
        )
        node.remove_node()
        return 0
    else:
        return -1
