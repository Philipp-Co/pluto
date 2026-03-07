

class PlutoNode:

    def setup(self, *args, **kwargs):
        print(f"--> Setup from Handler! {args}, {kwargs}")
        pass

    def teardown(self, *args, **kwargs):
        print(f"Teardown from Handler! {args}, {kwargs}")
        pass

    def run(self, id: int, event: int, number_of_output_queues: int, payload: str) -> str:
        print(f'Python: Hello from run: {id}, {event}, {number_of_output_queues}, {payload}')
        return (id, event, 0xffffffffffffffff, ('{' + f'\"python\":\"{payload}\"' + '}').encode())

    pass

