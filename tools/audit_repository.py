#!/usr/bin/env python3
"""Audit public files and optionally verify locally preserved evidence/artifacts.

No hardware, network, generated analysis or baseline regeneration is involved.
Missing acquired data in a public clone is reported separately from corruption.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
from urllib.parse import unquote

ROOT = Path(__file__).resolve().parents[1]
PUBLIC_DOCS = ('README.md', 'LICENSE.md', 'THIRD_PARTY_NOTICES.md', 'docs/README.md',
               'docs/BUILDING.md', 'docs/UPDATING.md', 'docs/THEMES.md',
               'docs/RESEARCH.md', 'docs/LEGAL_AND_SAFETY.md',
               'docs/WORKSPACE_MIGRATION.md', 'docs/gallery/README.md', 'artifacts/README.md')


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--require-artifacts', action='store_true')
    ap.add_argument('--out', type=Path)
    args = ap.parse_args()
    raw = subprocess.check_output(['git', '-C', str(ROOT), 'ls-files', '-co',
                                   '--exclude-standard', '-z'])
    names = sorted(set(raw.decode().strip('\0').split('\0')) - {''})
    errors, missing = [], []
    sep, username = '/', r'[^/\s"\x27]+/'
    private_path = re.compile('|'.join((sep + 'home' + sep + username,
        sep + 'Users' + sep + username,
        r'[A-Za-z]:' + re.escape(chr(92)) + 'Users' + re.escape(chr(92)) + r'[^\\\s".<>]+\\',
        sep + 'mnt' + sep + r'[a-z]' + sep + 'Users' + sep + username,
        sep + 'mnt' + sep + 'Games' + sep + 'Projects' + sep)))
    secret = re.compile(r'(?:gh[pousr]_[A-Za-z0-9]{30,}|github_pat_[A-Za-z0-9_]{40,}'
                        r'|-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----)')
    for name in names:
        path = ROOT / name
        if path.is_symlink():
            target = path.readlink()
            if target.is_absolute() or not path.resolve().is_relative_to(ROOT):
                errors.append({'file': name, 'error': 'symlink escapes repository'})
            continue
        if not path.is_file():
            errors.append({'file': name, 'error': 'missing public file'})
            continue
        try:
            text = path.read_text()
        except UnicodeError:
            continue
        if private_path.search(text):
            errors.append({'file': name, 'error': 'personal absolute path'})
        if secret.search(text):
            errors.append({'file': name, 'error': 'possible private key/token'})
    for name in PUBLIC_DOCS:
        path = ROOT / name
        if not path.is_file():
            errors.append({'file': name, 'error': 'missing primary document'})
            continue
        for target in re.findall(r'!?\[[^\]]*\]\(([^)]+)\)', path.read_text()):
            target = unquote(target.split('#', 1)[0].split(' "', 1)[0])
            if not target or '://' in target or target.startswith('mailto:'):
                continue
            dest = path.parent / target
            if not dest.exists():
                # Binary/data destinations deliberately excluded from public clones.
                if dest.resolve().is_relative_to(ROOT / 'artifacts'):
                    continue
                errors.append({'file': name, 'error': 'broken local link', 'target': target})
    checks = []
    for name, expected in json.loads((ROOT / 'docs/integration/evidence_hashes.json').read_text()).items():
        path = ROOT / name
        if not path.is_file():
            if path.is_symlink() or path.resolve().is_relative_to(ROOT / 'artifacts'):
                missing.append(name)
            else:
                errors.append({'file': name, 'error': 'missing original analysis evidence'})
            continue
        if digest(path) != expected:
            errors.append({'file': name, 'error': 'original evidence hash changed'})
        checks.append(name)
    catalog = json.loads((ROOT / 'artifacts/catalog.json').read_text())
    artifact_checks = 0
    for item in catalog['files']:
        path = ROOT / item['path']
        if not path.is_file():
            missing.append(item['path'])
        elif digest(path) != item['sha256']:
            errors.append({'file': item['path'], 'error': 'artifact hash changed'})
        else:
            artifact_checks += 1
    if args.require_artifacts and missing:
        errors.append({'error': 'required local artifacts absent', 'count': len(missing)})
    report = {'pass': not errors, 'public_files': len(names), 'errors': errors,
              'original_evidence_checked': len(checks), 'artifact_files_checked': artifact_checks,
              'optional_local_files_absent': len(missing),
              'scope': 'path/token patterns, primary-doc links, SHA-256; not a legal audit or hardware test'}
    if args.out:
        args.out.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
    return 0 if report['pass'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
