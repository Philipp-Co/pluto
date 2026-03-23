"""
CLI tool for managing nodes of a Pluto instance.
Supports adding and removing nodes.
"""
# ----------------------------------------------------------------------------------------------------------------------

from argparse import ArgumentParser
from logging import getLogger, Logger, StreamHandler, Formatter
from plyto.client.client import PlytoClient
from os import environ
from sys import argv

# ----------------------------------------------------------------------------------------------------------------------

def cli():
    #
    # -----------------------------------------------------
    #
    logger: Logger = getLogger()
    log_level: str = environ.get(
        'PLYTO_LOG_LEVEL',
        'INFO'
    )
    logger.setLevel(log_level)
    logging_handler: StreamHandler = StreamHandler()
    logging_handler.setFormatter(
        Formatter()
    )
    logger.addHandler(
        logging_handler
    )
    #
    # -----------------------------------------------------
    #
    parser: ArgumentParser = ArgumentParser()
    parser.add_argument(
        '-a',
        '--add-node',
        action='store_true',
    )
    parser.add_argument(
        '-r',
        '--remove-node',
        action='store_true',
    )
    parser.add_argument(
        '-n',
        '--name',
        type=str,
        default=None,
    )
    parser.add_argument(
        '-A',
        '--path-to-archive',
        type=str,
        default=None,
    )
    parser.add_argument(
        '-t',
        '--top-level-package-name',
        type=str,
        default=None,
    )
    parser.add_argument(
        '-u',
        '--user-arguments',
        type=str,
        default=None,
    )
    parser.add_argument(
        '-i',
        '--ip-address',
        type=str,
        default=None,
    )
    parser.add_argument(
        '-p',
        '--port',
        type=int,
        default=None,
    )
    args = parser.parse_args(argv[1:len(argv)])
    #
    # -----------------------------------------------------
    #
    if args.add_node and args.remove_node:
        logger.error(
            'Only one operation at a time is allowed.'
        )
        exit(-1)
    elif args.add_node:
        if args.path_to_archive is None:
            logger.error(
                'To add a new Node you have to specifiy a Path to the Python-Archive.'
            )
            exit(-1)
        client: PlytoClient = PlytoClient(logger.getChild(
            PlytoClient.__name__)
        ).connect(
            address=args.ip_address,
            port=args.port,
        )
        client.manager().add_node(
            name=args.name,
            python_archive=args.path_to_archive,
            top_level_package_name=args.top_level_package_name,
            user_arguments=args.user_arguments,
        )
        client.close()
    elif args.remove_node:
        client: PlytoClient = PlytoClient(logger.getChild(
            PlytoClient.__name__)
        ).connect(
            address=args.ip_address,
            port=args.port,
        )
        client.manager().remove_node(
            args.name,
        )
        client.close()
    else:
        logger.error(
            'Nothing to do...'
        )
        exit(-1)
    exit(0)

if __name__ == '__main__':
    cli()

# ----------------------------------------------------------------------------------------------------------------------
