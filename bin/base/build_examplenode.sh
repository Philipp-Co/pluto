#!/bin/sh
#
# Build the examplenode Python source distribution.
#
# Creates a virtual environment in pluto_internal_nodes/examplenode/.venv if it
# does not already exist and installs the dependencies from requirements.txt.
# The resulting .tar.gz archive is placed in pluto_internal_nodes/examplenode/dist/.
#

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$SCRIPT_DIR/../.."
EXAMPLENODE_DIR="$REPO_ROOT/pluto_internal_nodes/examplenode"
VENV_DIR="$EXAMPLENODE_DIR/.venv"

if [ ! -d "$VENV_DIR" ]; then
    echo "Creating virtual environment in $VENV_DIR ..."
    python3 -m venv "$VENV_DIR"
    echo "Installing dependencies from requirements.txt ..."
    "$VENV_DIR/bin/pip" install -r "$EXAMPLENODE_DIR/requirements.txt"
fi

"$VENV_DIR/bin/python" -m build --sdist "$EXAMPLENODE_DIR"
