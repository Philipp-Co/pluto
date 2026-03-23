"""
CLI tool for managing connections between nodes of a Pluto instance.
Supports connecting and disconnecting nodes.
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
        '-c',
        '--connect',
        action='store_true',
    )
    parser.add_argument(
        '-d',
        '--disconnect',
        action='store_true',
    )
    parser.add_argument(
        '-s',
        '--source',
        type=str,
        default=None,
    )
    parser.add_argument(
        '-t',
        '--target',
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
    if args.connect and args.disconnect:
        logger.error(
            'Only one operation at a time is allowed.'
        )
        exit(-1)
    elif args.connect:
        client: PlytoClient = PlytoClient(logger.getChild(
            PlytoClient.__name__)
        ).connect(
            address=args.ip_address,
            port=args.port,
        )
        client.manager().connect_nodes(
            src=args.source,
            dest=args.target,
        )
        client.close()
    elif args.disconnect:
        client: PlytoClient = PlytoClient(logger.getChild(
            PlytoClient.__name__)
        ).connect(
            address=args.ip_address,
            port=args.port,
        )
        client.manager().disconnect_nodes(
            src=args.source,
            dest=args.target,
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
