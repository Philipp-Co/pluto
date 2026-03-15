#!/bin/sh
#
# Prepares the benchmark environment.
#
# Builds the examplenode package, adds nodes and connects them.
#

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BASE_DIR="$SCRIPT_DIR/../base"
REPO_ROOT="$SCRIPT_DIR/../.."
EXAMPLENODE_DIST_DIR="$REPO_ROOT/pluto_internal_nodes/examplenode/dist"

"$BASE_DIR/build_examplenode.sh"

ARCHIVE="$(ls -t "$EXAMPLENODE_DIST_DIR"/*.tar.gz | head -1)"

"$BASE_DIR/add_node.sh" test "$ARCHIVE" examplenode
"$BASE_DIR/add_node.sh" test1 "$ARCHIVE" examplenode
"$BASE_DIR/connect_nodes.sh" test test1
"$BASE_DIR/connect_nodes.sh" test1 pluto_http_edge
"$BASE_DIR/start.sh"
