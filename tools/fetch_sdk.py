#!/usr/bin/env python3
"""Fetch a pinned, unmodified subset of the official WCH SDK; never accesses hardware."""
import concurrent.futures
import hashlib
import json
import shutil
from pathlib import Path
import urllib.request

ROOT = Path(__file__).resolve().parents[1]
REV = '080af2e52055334eaa6af0f7ad7df527b4ef5a66'
API = f'https://api.github.com/repos/openwch/ch32v307/git/trees/{REV}?recursive=1'

def get(url):
    with urllib.request.urlopen(url, timeout=60) as response:
        return response.read()

def main():
    tree = json.loads(get(API))
    wanted = [t['path'] for t in tree['tree'] if t['type'] == 'blob' and (
        t['path'].startswith('EVT/EXAM/SRC/') or
        t['path'] in ['LICENSE', 'README.md'] or
        t['path'].startswith('EVT/EXAM/GPIO/GPIO_Toggle/User/'))]
    def fetch(path):
        dest = ROOT / 'references/wch-sdk' / path
        dest.parent.mkdir(parents=True, exist_ok=True)
        if not dest.exists():
            dest.write_bytes(get(f'https://raw.githubusercontent.com/openwch/ch32v307/{REV}/{path}'))
        return {'path': path, 'sha256': hashlib.sha256(dest.read_bytes()).hexdigest()}
    with concurrent.futures.ThreadPoolExecutor(max_workers=6) as pool:
        files = list(pool.map(fetch, wanted))
    manifest = {'repository': 'https://github.com/openwch/ch32v307', 'revision': REV, 'files': files}
    (ROOT / 'references/wch-sdk-manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    shutil.copytree(ROOT/'references/wch-sdk',ROOT/'reconstructed/vendor/wch',dirs_exist_ok=True)
    shutil.copyfile(ROOT/'references/wch-sdk-manifest.json',ROOT/'reconstructed/vendor/wch-manifest.json')
    print(f'Fetched/verified {len(files)} official WCH SDK files at {REV}')

if __name__ == '__main__':
    main()
