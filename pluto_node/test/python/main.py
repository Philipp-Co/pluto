from time import time, ctime
from pluto import current_event, output_result
from json import dumps, loads

class Main:
    
    def __init__(self):
        self.__counter: int = 0
        pass

    def setup(self, argc, argv):
        print(f'Setup: {argc}, {argv}')
        pass

    def run(self):
        self.__counter += 1
        event: str = current_event().decode()
        output_result(
            dumps(
                {
                    'timestamp': ctime(time()),
                    'payload': event, 
                    'return_value': True,
                }
            )
        )
        print(f'Process Request {self.__counter} @ {ctime(time())}.')
        return self

    def teardown(self):
        print('Teardown')
        pass

    pass
