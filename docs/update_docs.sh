#!/bin/bash

set -e

# Build HTML documentation
python3 -m venv venv
source venv/bin/activate
python3 -m pip install -r docs/requirements.txt

cd "$(dirname "$0")"
python3 -m sphinx -b html . _build
