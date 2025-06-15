#!/bin/sh

set -e

# Build HTML documentation
cd "$(dirname "$0")"
python3 -m sphinx -b html . _build
