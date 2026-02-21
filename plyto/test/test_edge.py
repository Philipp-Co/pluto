from plyto.edge.plyto_edge import PlytoEdge, PlytoEdgeFactory
from plyto.config.plyto_config import PlytoConfig
from unittest import TestCase

from plyto.core.plyto_node import PlytoNode, PlytoNodeType
from plyto.core.plyto_core import PlytoCore

from multiprocessing import Process


class Initial(TestCase):

    def __run_plyto(self):

        self.__logger: Logger = getLogger()
        logger.setLevel('INFO')
        handler: StreamHandler = StreamHandler()
        handler.setFormatter(
            Formatter("%(asctime)s [%(levelname)-5.5s]  %(message)s")
        )

        self.__core: PlytoCore = PlytoCore(logger=logger)#.set_workdir('/Users/philippkroll/pluto/')

        # create Nodes
        node0: PlytoNode = PlytoNode(
            name='node0'
        ).set_executable(
            'plyto._internal.plyto_default_handler'
        ).set_type(
            PlytoNodeType.PYTHON
        )

        # Build Application
        self.__core.add(
            node0
        ).build_or_use_existing().exec()
        return 0

    def setUp(self):
        self.__process: Process = Process(
            target=self.__run_plyto,
            args=(,),
        )
        pass

    def tearDown(self):
        self.__process.terminate()
        self.__process.join()
        pass

    def test_initial(self) -> None:
        node_name: str = ''
        edge: PlytoEdge = PlytoEdgeFactory.as_input_to_node(
            node_name=node_name,
        )
        pass

    pass

