from logging import getLogger, Logger, StreamHandler, Formatter
from base64 import b64decode
from json import loads

class PlutoNode:
    """A Filter.

    This Node filters by Event-Id.
    """
    def __init__(self):
        self.__logger: Logger = getLogger('FilterNode') 
        handler: StreamHandler = StreamHandler()
        handler.setFormatter(
            Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        )
        self.__logger.addHandler(handler)
        self.__logger.setLevel('INFO')
        self.__filter = set()
        pass

    def __configure(self, configuration):
        self.__filter = set(
            configuration['event-ids']
        )
        pass

    def setup(self, *args, **kwargs):
        try:
            self.__logger.info(f'Setup: {args}, {kwargs}')
            configuration = loads(
                b64decode(
                    kwargs['configuration']
                ).decode()
            )
            self.__logger.info(
                f'Configure Isntance with {configuration}'
            )
            self.__configure(configuration)
            self.__logger.info(
                f'Let Event-Ids: {self.__filter} pass through.'
            )
        except Exception as e:
            self.__logger.exception(e)
            raise e
        pass

    def teardown(self, *args, **kwargs):
        self.__logger.info(f"Teardown from Handler! {args}, {kwargs}")
        pass

    def run(self, id: int, event: int, number_of_output_queues: int, payload: bytes) -> str:
        try:
            if event in self.__filter:
                return (id, event, 0xffffffffffffffff, payload)
            else:
                return (id, event, 0x0, b'')
        except Exception as e:
            self.__logger.exception(e)
            raise e
    pass

