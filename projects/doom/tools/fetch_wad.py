#!/usr/bin/env python3
"""Acquire the original shareware archive and verify/extract a local build input."""
import hashlib,io,json,urllib.request,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
URL='https://ftp.gwdg.de/pub/misc/ftp.idsoftware.com/idstuff/doom/doom19s.zip'
EXPECTED='1d7d43be501e67d927e415e0b8f3e29c3bf33075e859721816f652a526cac771'
def main():
    dest=ROOT.parents[1]/'artifacts/game-data/doom1.wad';archive=dest.parent/'doom19s.zip'
    if dest.exists():
        if hashlib.sha256(dest.read_bytes()).hexdigest()!=EXPECTED:
            raise SystemExit('Existing WAD differs; left untouched. Supply shareware 1.9 separately.')
        print('Pinned WAD already present')
    elif not archive.exists():
        with urllib.request.urlopen(URL,timeout=45) as response:raw=response.read(3_000_001)
        if len(raw)>3_000_000:raise SystemExit('Oversized archive')
        archive.parent.mkdir(parents=True,exist_ok=True);archive.write_bytes(raw)
    if not dest.exists():
        outer=zipfile.ZipFile(archive)
        payload=outer.read('DOOMS_19.1')+outer.read('DOOMS_19.2')
        inner=zipfile.ZipFile(io.BytesIO(payload));wad=inner.read('DOOM1.WAD')
        if hashlib.sha256(wad).hexdigest()!=EXPECTED:raise SystemExit('WAD hash mismatch; no WAD output created')
        dest.write_bytes(wad)
    wad=dest.read_bytes()
    report={'source':URL,'archive_sha256':hashlib.sha256(archive.read_bytes()).hexdigest() if archive.exists() else None,
            'wad_sha256':EXPECTED,'wad_bytes':len(wad),'terms':'id Software game data, not covered by code AGPL'}
    (ROOT/'assets/source.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
if __name__=='__main__':main()
