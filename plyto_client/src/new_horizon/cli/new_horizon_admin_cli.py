"""
CLI tool for administrating a Pluto instance.
Supports starting, stopping, and querying the state of the application.
"""

# ----------------------------------------------------------------------------------------------------------------------

from argparse import ArgumentParser
from logging import Formatter, Logger, StreamHandler, getLogger
from os import environ
from sys import argv
from sys import exit as sys_exit

from ..client.client import NewHorizonClient  # pylint: disable=relative-beyond-top-level

# ----------------------------------------------------------------------------------------------------------------------


def cli():
    """Entry point for the plyto_admin CLI tool."""
    #
    # -----------------------------------------------------
    #
    logger: Logger = getLogger()
    log_level: str = environ.get("PLYTO_LOG_LEVEL", "INFO")
    logger.setLevel(log_level)
    logging_handler: StreamHandler = StreamHandler()
    logging_handler.setFormatter(Formatter())
    logger.addHandler(logging_handler)
    #
    # -----------------------------------------------------
    #
    parser: ArgumentParser = ArgumentParser(description="Administration of a Pluto instance.")
    parser.add_argument(
        "-s",
        "--start",
        action="store_true",
        help="Start the application.",
    )
    parser.add_argument(
        "-x",
        "--stop",
        action="store_true",
        help="Stop the application.",
    )
    parser.add_argument(
        "-q",
        "--state",
        action="store_true",
        help="Query the current state of the application.",
    )
    parser.add_argument(
        "-i",
        "--ip-address",
        type=str,
        default=None,
        help="IP address of the Pluto server. Default: environment variable PLYTO_ADDRESS.",
    )
    parser.add_argument(
        "-p",
        "--port",
        type=int,
        default=None,
        help="Port of the Pluto server. Default: environment variable PLYTO_PORT.",
    )
    args = parser.parse_args(argv[1 : len(argv)])
    #
    # -----------------------------------------------------
    #
    client: NewHorizonClient
    if len([f for f in [args.start, args.stop, args.state] if f]) > 1:
        logger.error("Only one operation at a time is allowed.")
        sys_exit(-1)
    elif args.start:
        client = NewHorizonClient(logger.getChild(NewHorizonClient.__name__)).connect(
            address=args.ip_address,
            port=args.port,
        )
        client.manager().start()
        client.close()
    elif args.stop:
        client = NewHorizonClient(logger.getChild(NewHorizonClient.__name__)).connect(
            address=args.ip_address,
            port=args.port,
        )
        client.manager().stop()
        client.close()
    elif args.state:
        client = NewHorizonClient(logger.getChild(NewHorizonClient.__name__)).connect(
            address=args.ip_address,
            port=args.port,
        )
        state: str = client.manager().state()
        client.close()
        logger.info(state)
    else:
        logger.error("Nothing to do...")
        sys_exit(-1)
    sys_exit(0)


if __name__ == "__main__":
    cli()

# ----------------------------------------------------------------------------------------------------------------------
