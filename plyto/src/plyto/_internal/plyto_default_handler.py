from typing import Tuple

class PlutoEventHandler:
    
    def __init__(self):
        pass

    def setup(self, argc, argv):
        pass

    def teardown(self):
        pass

    def run(self, id: int, event: int, number_of_output_queues: int, payload: bytes) -> Tuple[int, int, int, bytes]:
        # result = '{' + f'\"python\":\"{payload}\"' + '}'
        print(
            'Payload ist %s' % (payload)
        )
        return (id, event, 0xffffffffffffffff, 'empty'.encode())

    pass

