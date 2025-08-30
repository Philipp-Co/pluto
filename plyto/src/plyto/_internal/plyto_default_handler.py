

class PlutoEventHandler:
    
    def __init__(self):
        pass

    def setup(self, argc, argv):
        pass

    def teardown(self):
        pass

    def run(self, id: int, event: int, number_of_output_queues: int, payload: str) -> str:
        result = '{' + f'\"python\":\"{payload}\"' + '}'
        return (id, event, 0xffffffffffffffff, result)

    pass

