from plyto.setup.nodes.node import Node
from logging import Logger
from typing_extensions import Self
from os import system


class PythonNode(Node):

    def __init__(self, logger: Logger, name: str, path_isntallable_package: str, package_name: str):
        super().__init__(logger, name)
        #
        # Path to an installable .tar.gz Archiv. 
        # I.e.: path/to/x.tar.gz
        # Die must be a file which is installable via pip.
        #
        self.__path_installable_package: str = path_isntallable_package
        #
        # Name of the Packge. 
        # Dies is the Importpath which is used by the Interpreter to import its Contents.
        # I.e.: from package_name.pluto import PlutoNode
        #
        self.__package_name: str = package_name
        pass
    
    @staticmethod
    def check_package_installable(path: str) -> bool:
        ending: str = 'tar.gz'
        if len(path) < len(ending):
            return False
        if path[len(path) - len(ending):] != ending:
            return False
        return True
    
    def check_interface(self) -> bool:
        from importlib import import_module
        
        self.logger().info(
            'Check Interface "%s"...' % (self.executable(),)
        )

        executable_module = self.executable()[:len(self.executable()) - len('PlutoNode') - 1]
        self.logger().info(
            'Load Module "%s"...' % (executable_module,)
        )
        
        #
        # Check if the Module is importable...
        #
        module = import_module(executable_module)
        self.logger().info(
            'Load Class PlutoNode from given Module...'
        )
        #
        # Check if the given Module contains the correct Class...
        #

        #
        # Check if the Class contains setup(), teardown() and run() Functions.
        # 

        self.logger().info(
            'Success.'
        )
        return True

    def create_venv(self) -> Self:
        self.logger().info(
           'Create venv for Node %s' % (self.name(),)
        ) 

        workdir: str = self.workdir() 
        system(
            f'cd {workdir} && python3 -m venv .venv'
        ) 

        self.logger().info(
           'Finish creating venv for Node %s' % (self.name(),)
        ) 
        return self

    def install(self) -> Self:
        self.logger().info(
           'Install Package "%s" into venv for Node %s' % (self.__path_installable_package, self.name(),)
        ) 
        retval: int = system(
            f'/pluto/nodes/{self.name()}/.venv/bin/pip install {self.__path_installable_package}'
        )
        if retval != 0:
            raise RuntimeError(
                'Unable to install Packge into venv!'
            )
        self.logger().info(
           'Finished installing Package for Node %s' % (self.name(),)
        ) 
        return self

    def executable(self) -> str:
        return f'{self.__package_name}.pluto.main'
    
    def type(self) -> str:
        return 'python'

    def create_empty_config(self, user_arguments: str='') -> Self:
        super().set_configuration(
            {
                "work_dir": self.workdir(),
                "name_of_input_queue": f'{self.name()}_iq',
                "names_of_output_queues":[],
                "python_home": '/usr/bin', #f'/pluto/nodes/{self.name()}/.venv',
                "python_path": f'/usr/lib/python3.8:/pluto/nodes/{self.name()}/.venv/lib/python3.8/site-packages',
                "ipc_home": '/pluto/ipc/',
                "user_arguments": user_arguments,
            }
        )
        super().write_configuration()
        return self

    pass

