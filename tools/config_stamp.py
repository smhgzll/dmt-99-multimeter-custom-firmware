"""Update a build stamp only when compiler/options change."""
import sys
from pathlib import Path

path = Path(sys.argv[1])
content = '\n'.join(sys.argv[2:]) + '\n'
if not path.exists() or path.read_text() != content:
    path.write_text(content)
