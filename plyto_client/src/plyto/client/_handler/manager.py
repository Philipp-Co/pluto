"""
Handler for administrative operations on a Pluto instance via HTTP.
"""

# ----------------------------------------------------------------------------------------------------------------------

from base64 import b64encode
from http import HTTPStatus
from json import dumps, loads
from typing import Dict, List, Optional

from requests import Response, delete, get, post, put  # pylint: disable=import-error
from typing_extensions import Self

from ...exceptions.exceptions import (  # pylint: disable=relative-beyond-top-level
    PlytoException,
    PlytoRequestFailedException,
)

# ----------------------------------------------------------------------------------------------------------------------


class PlytoManager:
    """Handler for administrative operations on a Pluto instance."""

    def __init__(self):
        """Initialize the manager."""
        self.__address: str = ""
        pass

    def __delf__(self):
        self.close()
        pass

    def connect(self, address: str) -> Self:
        """Connect to the Pluto server."""
        self.__address = address
        return self

    def close(self) -> Self:
        """Close the connection."""
        return self

    def connect_nodes(self, src: str, dest: str) -> Self:
        """Connect two nodes by name."""
        response: Response = put(
            url=f"http://{self.__address}/node/connect/",
            json={
                "source_name": src,
                "target_name": dest,
            },
        )
        if HTTPStatus.OK != response.status_code:
            raise PlytoException
        result = loads(response.content.decode())
        if not result["result"]:
            raise PlytoRequestFailedException(result["description"])
        return self

    def disconnect_nodes(self, src: str, dest: str) -> Self:
        """Disconnect two nodes by name."""
        response: Response = delete(
            url=f"http://{self.__address}/node/connect/",
            json={
                "source_name": src,
                "target_name": dest,
            },
        )
        if HTTPStatus.OK != response.status_code:
            raise PlytoException
        result = loads(response.content.decode())
        if not result["result"]:
            raise PlytoRequestFailedException(result["description"])
        return self

    def __upload_archive(self, url: str, content: str) -> Self:
        #
        # "{\"content\":\"$(cat $2 | openssl base64 -A)\"}"
        #
        response: Response = put(
            url=url,
            data=dumps(
                {
                    "content": content,
                }
            ),
        )
        if HTTPStatus.OK != response.status_code:
            raise PlytoException
        result = loads(response.content.decode())
        if not result["result"]:
            raise PlytoRequestFailedException(result["description"])
        return self

    def __add_node(
        self, url: str, top_level_package_name: str, user_arguments: str, custom_archive_name: Optional[str] = None
    ) -> Self:
        #
        # "{\"top_level_package_name\":\"$3\",\"user_arguments\":\"${4:-}\"}"
        #
        response: Response = put(
            url=url,
            json={
                "top_level_package_name": top_level_package_name,
                "user_arguments": user_arguments,
            }
            | (
                {}
                if custom_archive_name is None
                else {
                    "custom_archive_name": custom_archive_name,
                }
            ),
        )
        if HTTPStatus.OK != response.status_code:
            raise PlytoException(f"Response Code was {response.status_code}")
        result = loads(response.content.decode())
        if not result["result"]:
            raise PlytoRequestFailedException(result["description"])
        return self

    def add_node(self, name: str, python_archive: str, top_level_package_name: str, user_arguments: str) -> Self:
        """Add a new node to the Pluto instance."""
        try:
            url_archive: str = f"http://{self.__address}/manage/node/add/{name}/package/"
            url_add: str = f"http://{self.__address}/manage/node/add/{name}/"
            with open(python_archive, "rb") as file:
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
        """Remove a node from the Pluto instance by name."""
        try:
            url: str = f"http://{self.__address}/manage/node/add/{name}/"
            response: Response = delete(
                url=url,
            )
            if HTTPStatus.OK != response.status_code:
                raise PlytoException
            result = loads(response.content.decode())
            if not result["result"]:
                raise PlytoRequestFailedException(result["description"])
        except PlytoException as e:
            raise e
        except Exception as e:
            raise PlytoException from e
        return self

    def start(self) -> Self:
        """Start the Pluto application."""
        try:
            url: str = f"http://{self.__address}/manage/"
            response: Response = post(
                url=url,
            )
            if HTTPStatus.OK != response.status_code:
                raise PlytoRequestFailedException
        except Exception as e:
            raise PlytoException from e
        return self

    def stop(self) -> Self:
        """Stop the Pluto application."""
        try:
            url: str = f"http://{self.__address}/manage/"
            response: Response = delete(
                url=url,
            )
            if HTTPStatus.OK != response.status_code:
                raise PlytoRequestFailedException
        except Exception as e:
            raise PlytoException from e
        return self

    def state(self) -> str:
        """Query the current state of the Pluto application."""
        try:
            url: str = f"http://{self.__address}/manage/"
            response: Response = get(
                url=url,
            )
            if HTTPStatus.OK != response.status_code:
                raise PlytoRequestFailedException
            result = loads(response.content.decode())
            return result["state"]
        except Exception as e:
            raise PlytoException from e

    def nodes(self) -> Dict[str, List[str]]:
        """Query all nodes and their connections."""
        try:
            response: Response = get(
                f"http://{self.__address}/manage/node/",
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
                result = loads(response.content.decode())
                mat: Dict[str, List[str]] = {}
                for i in range(len(result["connections"])):
                    connection = result["connections"][i]
                    if connection["source"] not in mat:
                        mat[connection["source"]] = []
                    mat[connection["source"]].append(connection["target"])
                return mat
        except Exception as e:
            raise PlytoException from e
        raise PlytoRequestFailedException

    pass


# ----------------------------------------------------------------------------------------------------------------------
