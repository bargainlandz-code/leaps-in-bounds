"""Direct regression tests for degree-diameter hamming4line witness routing."""
from pathlib import Path
import sys
import tempfile
import subprocess
import yaml

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
from certificates import check_witness, CertificateError

cert_path = ROOT / "certificates/degree-diameter/degree-diameter-17-5-lower-167680.yml"
cert = yaml.safe_load(cert_path.read_text())
cert["path"] = str(cert_path)
assert cert["constant"] == "degree-diameter-17-5"
assert cert["witness"]["format"] == "hamming4line"
assert (cert_path.parent / cert["witness"]["file"]).is_file()
assert cert_path.stat().st_size + (cert_path.parent / cert["witness"]["file"]).stat().st_size < 500000

# Test the REAL family dispatcher, not a stand-alone helper.
invalid = dict(cert)
invalid["value"] = 167681
try:
    check_witness(invalid)
except CertificateError as exc:
    assert "hamming4line witness rejected" in str(exc), str(exc)
else:
    raise AssertionError("wrong order was accepted")

invalid2 = dict(cert)
invalid2["constant"] = "degree-diameter-17-4"
try:
    check_witness(invalid2)
except CertificateError as exc:
    assert "hamming4line witness rejected" in str(exc), str(exc)
else:
    raise AssertionError("wrong diameter was accepted")

# Negative input tests for independent reconstruction, with bounded BFS time.
cpp = ROOT / "scripts/degree_diameter_hamming4line.cpp"
binary = cert_path.parent / cert["witness"]["file"]
with tempfile.TemporaryDirectory() as tmp:
    exe = Path(tmp)/"check"
    subprocess.run(["g++","-std=c++17","-O3",str(cpp),"-o",str(exe)], check=True)
    original = binary.read_bytes()
    tests = [
        ("bad magic", b"BADSIGN!"+original[8:]),
        ("truncated", original[:-2]),
        ("broken line matching", original[:17]+bytes([original[17]^1])+original[18:]),
    ]
    for label, payload in tests:
        file = Path(tmp)/"bad.h4l"
        file.write_bytes(payload)
        result = subprocess.run([str(exe),str(file),"167680","17","5","0","1"],capture_output=True,text=True,timeout=60)
        assert result.returncode != 0, label
print("Direct dispatcher and malformed-witness tests passed")
