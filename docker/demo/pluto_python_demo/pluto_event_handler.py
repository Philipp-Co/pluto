
import sys

class PlutoEventHandler:

    def __init__(self):
        print('Ctor!')
        pass

    def setup(self, argc, argv):
        print(f"--> Setup from Handler! argc: {argc}, argv: {argv}")
        pass

    def teardown(self):
        print("Teardown from Handler!")
        pass

    def run(self, id: int, event: int, number_of_output_queues: int, payload: str) -> str:
        print(f'Python: Hello from run: {id}, {event}, {number_of_output_queues}, {payload}')
        return (id, event, 0xffffffffffffffff, '{' + f'\"python\":\"{payload}\"' + '}')

    pass

