
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

    def run(self, id: int, event: int, payload: str) -> str:
        print(f'Hello from run: {id}, {event}, {payload}')
        return '{' + f'\"python\":\"{payload}\"' + '}'

    pass

