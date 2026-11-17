"""Bridge from the degree-diameter certificate dispatcher to a C++ witness checker.

This only applies to `format: hamming4line`; all other graph families and formats
continue through their original verification functions. No network access.
"""
from __future__ import annotations

import pathlib
import subprocess
import tempfile
from typing import Any, Mapping


def check_hamming4line(params: Mapping[str, int], value: int, cert: Mapping[str, Any]) -> None:
    witness = cert.get('witness', {})
    if witness.get('format') != 'hamming4line':
        raise ValueError('wrong witness format')
    if 'file' not in witness or 'url' in witness or 'data' in witness:
        raise ValueError('hamming4line witness must be stored in a local binary file')
    certificate_path = pathlib.Path(cert['path']).resolve()
    base_dir = certificate_path.parent
    witness_file = (base_dir / str(witness['file'])).resolve()
    if not witness_file.is_relative_to(base_dir):
        raise ValueError('witness file escapes certificate directory')
    if not witness_file.is_file():
        raise FileNotFoundError(str(witness_file))
    cpp = pathlib.Path(__file__).with_name('degree_diameter_hamming4line.cpp')
    if not cpp.is_file():
        raise FileNotFoundError('missing hamming4line checker source')
    with tempfile.TemporaryDirectory(prefix='lib-hamming4line-') as tmp:
        exe = pathlib.Path(tmp)/'hamming4line'
        subprocess.run(['g++','-std=c++17','-O3','-Wall','-Wextra',str(cpp),'-o',str(exe)],
                       check=True,capture_output=True,text=True,timeout=90)
        result = subprocess.run([str(exe),str(witness_file),str(value),str(params['d']),str(params['k'])],
                                capture_output=True,text=True,timeout=3600)
        if result.returncode != 0:
            raise ValueError(f'hamming4line witness rejected ({result.returncode}): '+(result.stderr or result.stdout)[-2000:])
        if 'all_roots=yes' not in result.stdout or 'RECONSTRUCTION_PASS' not in result.stderr:
            raise ValueError('hamming4line checker did not confirm exhaustive validation')
        print(result.stdout.strip())
