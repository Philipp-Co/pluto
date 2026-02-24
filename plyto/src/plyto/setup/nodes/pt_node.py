from plyto.setup.nodes.node import Node
from typing_extensions import Self


class PassthroughNode(Node):

    def executable(self) -> str:
        return ''
    
    def type(self) -> str:
        return 'passthrough'
    
    def create_empty_config(self) -> Self:
        super().set_configuration(
            {
                "work_dir": self.workdir(),
                "name_of_input_queue": f'{self.name()}_iq',
                "names_of_output_queues":[],
                "ipc_home": '/pluto/ipc/',
            }
        )
        super().write_configuration()
        return self

    pass

