from logging import getLogger, Logger
from base64 import b64decode
from json import loads


class PlutoNode:
    """Implements a Router.

    A Router maps Events with a specific Event-Id to a specific Output Queue.
    """    

    def __init__(self):
        self.__logger: Logger = getLogger(self.__class__.__name__) 
        self.__logger.setLevel('INFO')
        self.__mapping = {}
        pass

    def setup(self, *args, **kwargs):
        print(f"--> Setup from Handler! {args}, {kwargs}")
        if 'configuration' in kwargs:
            config = loads(
                b64decode(
                    kwargs['configuration']
                )
            )
            for item in config['mapping']:
                if item['event-id'] not in self.__mapping:
                    self.__mapping[item['event-id']] = 0
                self.__mapping[item['event-id']] = self.__mapping[item['event-id']] | (1 << item['output'])
        else:
            self.__logger.warning(
                'No Configuration given!'
            )
        pass

    def teardown(self, *args, **kwargs):
        print(f"Teardown from Handler! {args}, {kwargs}")
        pass

    def run(self, id: int, event: int, number_of_output_queues: int, payload: bytes) -> str:
        self.__logger.info(f'Python: Hello from run: {id}, {event}, {number_of_output_queues}, {payload}')
        self.__logger.info(f'Type of Payload: {type(payload)}')
        output_queue: int = int(
            self.__mapping[event]
        )
        return (id, event, output_queue, payload)

    pass

