from logging import Logger
from typing_extensions import Self
from typing import Any
from json import dumps, loads
from os import makedirs, removedirs
from abc import ABC, abstractmethod


class Node:

    def __init__(self, logger: Logger, name: str):
        self.__logger: Logger = logger
        self.__configuration: Any = {}
        self.__name: str = name
        self.__workdir = f'/pluto/nodes/{self.__name}/'
        self.__logger.info(
            'Create Node "%s"' % (self.__name,)
        )
        pass
    
    def core_configuration_file(self) -> str:
        return '/pluto/core/config/core.cfg'

    def set_workdir(self, workdir: str) -> Self:
        if workdir[len(workdir)-1] != '/' or workdir[0] != '/':
            raise ValueError(
                f'The given Workdir {workdir} is not valid! It must be a absolute Directory with a trailing /. F.e.: "/path/to/work/dir/".'
            )
        self.__workdir = workdir
        return self

    def logger(self) -> Logger:
        return self.__logger

    def name(self) -> str:
        return self.__name

    @abstractmethod
    def executable(self) -> str:
        pass

    @abstractmethod
    def type(self) -> str:
        pass

    def workdir(self) -> str:
        return self.__workdir
    
    def configuration_dir(self) -> str:
        return f'{self.workdir()}config/'

    def configuration_file(self) -> str:
        return f'{self.configuration_dir()}{self.name()}.cfg'

    def write_configuration(self, **kwargs) -> Self:
        #
        # Register this Node in Core...
        #
        core_config = {}
        with open(self.core_configuration_file(), 'r') as file:
            core_config = loads(file.read())
        core_config['nodes'].append(
            {
                **{
                    "type": self.type(),
                    "name": self.name(),
                    "configuration-file": self.configuration_file(),
                    "executable": self.executable(),
                }, 
                **kwargs
            }
        )
        with open(self.core_configuration_file(), 'w+') as file:
            file.write(f'{dumps(core_config)}\n')
        #
        # Create own Directory for this Node.
        #
        self.__logger.info(
            'Write Config "%s" to "%s".' % (dumps(self.__configuration), self.configuration_file(),)
        )
        makedirs(self.configuration_dir(), exist_ok=True)
        with open(self.configuration_file(), 'w+') as file:
            file.write(f'{dumps(self.__configuration)}\n')
        return self

    def set_configuration(self, serializable) -> Self:
        self.__configuration = serializable
        return self

    def remove_node(self) -> Self:
        core_config = {}
        with open(self.core_configuration_file(), 'r') as file:
            core_config = loads(file.read())
        core_config['nodes'] = [node for node in core_config['nodes'] if node['name'] != self.name()]
        with open(self.core_configuration_file(), 'w+') as file:
            file.write(f'{dumps(core_config)}\n')
        
        from shutil import rmtree
        rmtree(self.workdir())
        return self

    pass

