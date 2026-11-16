"""Regression tests for compact degree-diameter metacyclic witnesses.

Run: python scripts/test_metacyclic_witness.py
"""
import json
from pathlib import Path

import yaml
from certificates import CertificateError, degree_diameter_check_metacyclic


for degree, size in ((21, 358092), (22, 445788)):
    path = Path(f"certificates/degree-diameter/degree-diameter-{degree}-5-lower-{size}.yml")
    cert = yaml.safe_load(path.read_text())
    data = cert["witness"]["data"]
    params = {"d": degree, "k": 5}
    degree_diameter_check_metacyclic(params, size, data)

    witness = json.loads(data)
    failures = [
        dict(witness, generators=witness["generators"][:-1]),
        dict(witness, generators=witness["generators"][:-1] + [witness["generators"][0]]),
        dict(witness, n=witness["n"] - 1),
        dict(witness, r=31),
    ]
    for malformed in failures:
        try:
            degree_diameter_check_metacyclic(params, size, json.dumps(malformed))
        except CertificateError:
            continue
        raise AssertionError(f"Malformed metacyclic witness was accepted for N({degree},5)")

    print(f"N({degree},5) = {size}: valid witness passed; four corruptions rejected")
