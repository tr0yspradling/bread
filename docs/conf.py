import os
import sys

# Add project root to sys.path
sys.path.insert(0, os.path.abspath('..'))

project = 'Bread'
author = 'Bread Developers'

extensions = ['myst_parser']

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}

html_theme = 'alabaster'

master_doc = 'index'

# Output directory is docs/_build
