#!/usr/bin/env python3
"""Filter CodeQL findings by source location."""

from __future__ import annotations

from pathlib import Path
from typing import Any, cast
from urllib.parse import urljoin
from urllib.request import url2pathname
import json
import os
import sys

__all__ = ()
_PATHS: tuple[str, ...] = ('src', 'tests', 'dbeaver_creds')
_EXCLUDED = frozenset(('.vcpkg', '3rdparty'))
_SARIF_ARGUMENT_COUNT = 3


def _in_scope(filename: str) -> bool:
    """
    Check a source location against project paths.

    Parameters
    ----------
    filename : str
        Absolute or repository-relative source path.

    Returns
    -------
    bool
    """
    root = Path(os.environ.get('GITHUB_WORKSPACE', '.')).resolve()
    source = Path(filename).resolve()
    try:
        relative = source.relative_to(root)
    except ValueError:
        return False
    return not _EXCLUDED.intersection(relative.parts) and any(
        source == allowed or allowed in source.parents
        for allowed in ((root / path).resolve() for path in _PATHS))


def _artifact_uri(artifact: dict[str, Any], run: dict[str, Any]) -> str | None:
    """
    Resolve a SARIF artifact URI and its base chain.

    Parameters
    ----------
    artifact : dict[str, Any]
        Artifact location.
    run : dict[str, Any]
        SARIF run with artifact and URI base tables.

    Returns
    -------
    str | None

    Raises
    ------
    RuntimeError
        When URI bases include a cycle.
    """
    if 'uri' not in artifact and 'index' in artifact:
        artifact = run['artifacts'][artifact['index']]['location']
    if 'uri' not in artifact:
        return None
    uri = artifact['uri']
    base_id = artifact.get('uriBaseId', '')
    seen = set()
    while base_id:
        if base_id in seen:
            msg = 'Recursive SARIF URI base.'
            raise RuntimeError(msg)
        seen.add(base_id)
        base = run.get('originalUriBaseIds', {}).get(base_id, {})
        uri = urljoin(base.get('uri', ''), uri)
        base_id = base.get('uriBaseId', '')
    return cast('str', uri.removeprefix('file:'))


def _filter_sarif(path: Path) -> None:
    """
    Restrict findings by their primary source locations.

    Parameters
    ----------
    path : Path
        SARIF file or output directory.

    Raises
    ------
    RuntimeError
        When analysis produced no SARIF files or URI bases include a cycle.
    """
    files = list(path.rglob('*.sarif')) if path.is_dir() else [path]
    if not files:
        msg = 'CodeQL produced no SARIF files.'
        raise RuntimeError(msg)
    for filename in files:
        data = json.loads(filename.read_text(encoding='utf-8'))
        for run in data['runs']:
            results = []
            for result in run.get('results', []):
                locations = []
                for location in result.get('locations', []):
                    artifact = location.get('physicalLocation', {}).get('artifactLocation', {})
                    if (uri := _artifact_uri(artifact, run)) is not None:
                        locations.append(_in_scope(url2pathname(uri)))
                if not locations or any(locations):
                    results.append(result)
            sys.stdout.write(f'{filename}: {len(results)} project findings; '
                             f'{len(run.get("results", [])) - len(results)} '
                             'outside configured scope.\n')
            run['results'] = results
        filename.write_text(json.dumps(data, indent=2) + '\n', encoding='utf-8')


if __name__ == '__main__':
    if len(sys.argv) == _SARIF_ARGUMENT_COUNT and sys.argv[1] == '--sarif':
        _filter_sarif(Path(sys.argv[2]))
    else:
        msg = 'Usage: static-analysis.py --sarif FILE_OR_DIRECTORY'
        raise SystemExit(msg)
