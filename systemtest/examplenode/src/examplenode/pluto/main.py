from logging import getLogger, Logger


class PlutoNode:
    
    def __init__(self):
        self.__logger: Logger = getLogger(self.__class__.__name__) 
        self.__logger.setLevel('INFO')
        pass

    def setup(self, *args, **kwargs):
        print(f"--> Setup from Handler! {args}, {kwargs}")
        pass

    def teardown(self, *args, **kwargs):
        print(f"Teardown from Handler! {args}, {kwargs}")
        pass

    def run(self, id: int, event: int, number_of_output_queues: int, payload: bytes) -> str:
        self.__logger.info(f'Python: Hello from run: {id}, {event}, {number_of_output_queues}, {payload}')
        self.__logger.info(f'Type of Payload: {type(payload)}')
        return (id, event, 0xffffffffffffffff, payload)

    pass

