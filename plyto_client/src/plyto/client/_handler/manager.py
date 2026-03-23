"""
Handler for administrative operations on a Pluto instance via HTTP.
"""
# ----------------------------------------------------------------------------------------------------------------------

from json import dumps, loads
from base64 import b64encode
from plyto.exceptions.exceptions import PlytoException, PlytoRequestFailedException
from requests import post, put, get, delete, Response
from http import HTTPStatus
from threading import Thread
from queue import Queue
from time import sleep
from typing_extensions import Self
from typing import Optional
from logging import Logger

# ----------------------------------------------------------------------------------------------------------------------

class PlytoManager:

    def __init__(self):
        self.__address: str = ''
        pass

    def __delf__(self):
        self.close()
        pass

    def connect(self, address: str) -> Self:
        self.__address = address
        return self

    def close(self) -> Self:
        return self

    def connect_nodes(self, src: str, dest: str) -> Self:
        response: Response = put(
            url=f'http://{self.__address}/node/connect/',
            json={
                'source_name': src,
                'target_name': dest,
            },
        )
        if HTTPStatus.OK != response.status_code:
            raise PlytoException
        result = loads(response.content.decode())
        if not result['result']:
            raise PlytoRequestFailedException(result['description'])
        return self

    def disconnect_nodes(self, src: str, dest: str) -> Self:
        response: Response = delete(
            url=f'http://{self.__address}/node/connect/',
        )
        if HTTPStatus.OK != response.status_code:
            raise PlytoException
        result = loads(response.content.decode())
        if not result['result']:
            raise PlytoRequestFailedException(result['description'])
        return self

    def __upload_archive(self, url: str, content: str) -> Self:
        #
        # "{\"content\":\"$(cat $2 | openssl base64 -A)\"}"
        #
        response: Response = put(
            url=url,
            data=dumps(
                {
                    'content': content,
                }
            ),
        )
        if HTTPStatus.OK != response.status_code:
            raise PlytoException
        result = loads(response.content.decode())
        if not result['result']:
            raise PlytoRequestFailedException(result['description'])
        return self

    def __add_node(self, url: str, top_level_package_name: str, user_arguments: str, custom_archive_name: Optional[str] = None) -> Self:
        #
        # "{\"top_level_package_name\":\"$3\",\"user_arguments\":\"${4:-}\"}"
        #
        response: Response = put(
            url=url,
            json={
                'top_level_package_name': top_level_package_name,
                'user_arguments': user_arguments,
            } | (
                {} if custom_archive_name is None else {'custom_archive_name': custom_archive_name,}
            ),
        )
        if HTTPStatus.OK != response.status_code:
            raise PlytoException(f'Response Code was {response.status_code}')
        result = loads(response.content.decode())
        if not result['result']:
            raise PlytoRequestFailedException(result['description'])
        return self

    def add_node(self, name: str, python_archive: str, top_level_package_name: str, user_arguments: str) -> Self:
        try:
            url_archive: str = f'http://{self.__address}/manage/node/add/{name}/package/'
            url_add: str = f'http://{self.__address}/manage/node/add/{name}/'
            with open(python_archive, 'rb') as file:
                archive_content: bytes = file.read()
                encoded_content: str = b64encode(archive_content).decode()
                self.__upload_archive(
                    url=url_archive,
                    content=encoded_content,
                )
                self.__add_node(
                    url=url_add,
                    top_level_package_name=top_level_package_name,
                    user_arguments=user_arguments,
                )
        except PlytoException as e:
            raise e
        except Exception as e:
            raise PlytoException from e
        return self

    def remove_node(self, name: str) -> Self:
        try:
            url: str = f'http://{self.__address}/manage/node/add/{name}/'
            response: Response = delete(
                url=url,
            )
            if HTTPStatus.OK != response.status_code:
                raise PlytoException
            result = loads(response.content.decode())
            if not result['result']:
                raise PlytoRequestFailedException(result['description'])
        except PlytoException as e:
            raise e
        except Exception as e:
            raise PlytoException from e
        return self

    def start(self) -> Self:
        try:
            url: str = f'http://{self.__address}/manage/'
            response: Response = post(
                url=url,
            )
            if HTTPStatus.OK != response.status_code:
                raise PlytoRequestFailedException
        except Exception as e:
            raise PlytoException from e
        return self

    def stop(self) -> Self:
        try:
            url: str = f'http://{self.__address}/manage/'
            response: Response = delete(
                url=url,
            )
            if HTTPStatus.OK != response.status_code:
                raise PlytoRequestFailedException
        except Exception as e:
            raise PlytoException from e
        return self

    def state(self) -> str:
        try:
            url: str = f'http://{self.__address}/manage/'
            response: Response = get(
                url=url,
            )
            if HTTPStatus.OK != response.status_code:
                raise PlytoRequestFailedException
            result = loads(
                response.content.decode()
            )
            return result['state']
        except Exception as e:
            raise PlytoException from e

    def nodes(self) -> Dict[str, List[str]]:
        try:
            response: Response = get(
                    f'http://{self.__address}/manage/node/',
            )
            #
            # {
            #  "result": true,
            #  "description": "...",
            #  "nodes": ["node_a", "node_b"],
            #  "connections": [
            #     {"source": "node_a", "target": "node_b"}
            #   ]
            # }
            #
            if response.status_code == HTTPStatus.OK:
                result = loads(
                    response.content.decode()
                )
                mat = {}
                for i in range(len(result['connections'])):
                    connection = result['connections'][i]
                    if connection['source'] not in mat:
                        mat[connection['source']] = []
                    mat[connection['source']].append(
                        connection['target']
                    )
                return mat
        except Exception as e:
            raise PlytoException from e
        raise PlytoRequestFailedException

    pass

# ----------------------------------------------------------------------------------------------------------------------
