import os
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
os.environ.setdefault('QT_QPA_PLATFORM', 'offscreen')

from dmt99studio import paths  # noqa: E402
from dmt99studio.core.flashimage import FlashImage  # noqa: E402


@pytest.fixture(scope='session')
def original_path():
    if not paths.ORIGINAL_DUMP.exists():
        pytest.skip('original Winbond dump not available')
    return paths.ORIGINAL_DUMP


@pytest.fixture()
def image(original_path):
    return FlashImage.load(original_path)
