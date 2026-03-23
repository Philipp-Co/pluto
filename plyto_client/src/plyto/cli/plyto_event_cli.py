"""
CLI tool for sending and receiving events on a Pluto instance.
"""
# ----------------------------------------------------------------------------------------------------------------------

from argparse import ArgumentParser
from logging import getLogger, Logger, StreamHandler, Formatter
from plyto.client.client import PlytoClient, PlytoEvent, PlytoReceiver, PlytoTransmitter
from plyto.exceptions.exceptions import PlytoNoEventAvailableExcpetion, PlytoException
from os import environ
from sys import argv
from time import sleep
from datetime import datetime, timezone
from json import loads, dumps

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
        '-s',
        '--send',
        action='store_true',
    )
    parser.add_argument(
        '-r',
        '--receive',
        action='store_true',
    )
    parser.add_argument(
        '-n',
        '--name',
        type=str,
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
    parser.add_argument(
        'events',
        nargs='*',
        type=str,
    )
    args = parser.parse_args(argv[1:len(argv)])
    print(args)
    #
    # -----------------------------------------------------
    #
    if args.send and args.receive:
        logger.error(
            'It is not allowed to send and receive at the same time...'
        )
        exit(-1)
    elif args.send:
        client: PlytoClient = PlytoClient(logger.getChild(
            PlytoClient.__name__)
        ).connect(
            address=args.ip_address,
            port=args.port,
        )
        t: PlytoTransmitter = client.transmitter()
        for i in range(len(args.events)):
            raw = loads(args.events[i])
            t.transmit(
                name=args.name,
                event=PlytoEvent(
                    id=raw['id'],
                    event_id=raw['event-id'],
                    timestamp=datetime.now(timezone.utc),
                    payload=raw['payload'],
                ),
            )
        client.close()
    elif args.receive:
        from signal import signal, SIGINT
        terminate: bool = False
        def signal_handler(*args, **kwargs):
            nonlocal terminate
            terminate = True
            pass
        signal(SIGINT, signal_handler)
        client: PlytoClient = PlytoClient(logger.getChild(
            PlytoClient.__name__)
        ).connect(
            address=args.ip_address,
            port=args.port,
        )
        r = client.receiver()
        while not terminate:
            try:
                event: PlytoEvent = r.receive()
                logger.info(
                    dumps(
                        {
                            'id': event.id,
                            'event-id': event.event_id,
                            'timestamp': event.timestamp.isoformat(),
                            'payload': event.payload,
                        }
                    )
                )
            except PlytoNoEventAvailableExcpetion:
                sleep(0.25)
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
