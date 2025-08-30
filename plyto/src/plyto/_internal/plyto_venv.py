from typing import Set


class PlytoPythonInterpreter:
    
    def __init__(self):
        super().__init__()
        pass
    
    def _exec(self, pipe, parent) -> None:
        import sys
        parent.close()
        pipe.send(
            sys.path
        )
        pipe.close()
        exit(0)
        pass

    def python_path(self) -> Set[str]:
        from multiprocessing import Process, Pipe
        from json import loads
        from os import system
        
        parent, child = Pipe()
        p = Process(target=self._exec, args=(child, parent, ))
        p.start()
        child.close()
        path = ['']
        while True:
            try:
                path = path + parent.recv()
            except EOFError:
                break
        p.join()
        return set(path) 

    pass
