"""
CLI tool for sending and receiving events on a Pluto instance.
"""

# ----------------------------------------------------------------------------------------------------------------------

from argparse import ArgumentParser
from datetime import datetime, timezone
from json import dumps, loads
from logging import Formatter, Logger, StreamHandler, getLogger
from os import environ
from signal import SIGINT, signal
from sys import argv
from sys import exit as sys_exit
from time import sleep

from ..client.client import (  # pylint: disable=relative-beyond-top-level
    NewHorizonClient,
    NewHorizonEvent,
    NewHorizonTransmitter,
)
from ..exceptions.exceptions import NewHorizonNoEventAvailableException  # pylint: disable=relative-beyond-top-level

# ----------------------------------------------------------------------------------------------------------------------


def cli():
    """Entry point for the plyto_event CLI tool."""
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
    parser: ArgumentParser = ArgumentParser()
    parser.add_argument(
        "-s",
        "--send",
        action="store_true",
    )
    parser.add_argument(
        "-r",
        "--receive",
        action="store_true",
    )
    parser.add_argument(
        "-n",
        "--name",
        type=str,
    )
    parser.add_argument(
        "-i",
        "--ip-address",
        type=str,
        default=None,
    )
    parser.add_argument(
        "-p",
        "--port",
        type=int,
        default=None,
    )
    parser.add_argument(
        "events",
        nargs="*",
        type=str,
    )
    args = parser.parse_args(argv[1 : len(argv)])
    print(args)
    #
    # -----------------------------------------------------
    #
    client: NewHorizonClient
    if args.send and args.receive:
        logger.error("It is not allowed to send and receive at the same time...")
        sys_exit(-1)
    elif args.send:
        client = NewHorizonClient(logger.getChild(NewHorizonClient.__name__)).connect(
            address=args.ip_address,
            port=args.port,
        )
        t: NewHorizonTransmitter = client.transmitter()
        for _, event_str in enumerate(args.events):
            raw = loads(event_str)
            t.transmit(
                name=args.name,
                event=NewHorizonEvent(
                    id=raw["id"],
                    event_id=raw["event-id"],
                    timestamp=datetime.now(timezone.utc),
                    payload=raw["payload"],
                ),
            )
        client.close()
    elif args.receive:
        terminate: bool = False

        def signal_handler(*_args, **_kwargs):
            nonlocal terminate
            terminate = True

        signal(SIGINT, signal_handler)
        client = NewHorizonClient(logger.getChild(NewHorizonClient.__name__)).connect(
            address=args.ip_address,
            port=args.port,
        )
        r = client.receiver()
        while not terminate:
            try:
                event: NewHorizonEvent = r.receive()
                logger.info(
                    dumps(
                        {
                            "id": event.id,
                            "event-id": event.event_id,
                            "timestamp": event.timestamp.isoformat(),
                            "payload": event.payload,
                        }
                    )
                )
            except NewHorizonNoEventAvailableException:
                sleep(0.25)
        client.close()
    else:
        logger.error("Nothing to do...")
        sys_exit(-1)
    sys_exit(0)


if __name__ == "__main__":

    cli()

# ----------------------------------------------------------------------------------------------------------------------
