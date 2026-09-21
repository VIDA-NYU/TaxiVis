"""Exercise the actual builder and reader, including malformed legacy indexes."""
import pathlib
import random
import struct
import subprocess
import sys
import tempfile

builder, checker, sample = sys.argv[1:]

def run(args, success=True):
    result = subprocess.run(args, text=True, capture_output=True, timeout=120)
    if success:
        assert result.returncode == 0, result.stdout + result.stderr
    else:
        assert result.returncode == 1, f"Expected clean rejection, got {result.returncode}: {result.stderr}"
    return result


def trip(seed):
    rng = random.Random(seed)
    return struct.pack('<IIffffIIIIHHHHHHHBB',
                       100 + rng.randrange(8), 200 + rng.randrange(8),
                       -74 + rng.randrange(8) / 10, 40 + rng.randrange(8) / 10,
                       -74 + rng.randrange(8) / 10, 40 + rng.randrange(8) / 10,
                       0, 0, 0, 0, rng.randrange(8), 100, 100, 0, 0, 0, 0, 0, 1)

with tempfile.TemporaryDirectory(prefix='taxivis-test-') as directory:
    root = pathlib.Path(directory)
    for name, records in [('single', [trip(0)]), ('pair', [trip(0)] * 2),
                          ('identical', [trip(0)] * 4096),
                          ('mixed', [trip(i) for i in range(512)])]:
        source, target = root / (name + '.trip'), root / (name + '.kdtrip')
        payload = b''.join(records)
        source.write_bytes(payload)
        run([builder, str(source), str(target)])
        assert source.read_bytes() == payload, 'Builder modified input'
        print(name, run([checker, str(target), str(len(records))]).stdout.strip())
    run([builder, str(source), str(source)], success=False)
    assert source.read_bytes() == payload
    # Builder errors: empty and partial records, missing input, bad output path.
    for content in [b'', b'x', trip(0) + b'x']:
        bad = root / 'bad.trip'
        bad.write_bytes(content)
        run([builder, str(bad), str(root / 'bad.kdtrip')], success=False)
    run([builder, str(root / 'missing'), str(root / 'bad.kdtrip')], success=False)
    run([builder, str(source), str(root / 'missing' / 'out')], success=False)

    valid = bytearray((root / 'pair.kdtrip').read_bytes())
    malformed = [b'', b'x', valid[:-1], valid[:-12], valid + bytes(12)]
    for offset in [2, len(valid) // 12, 2**64 - 2]:
        broken = bytearray(valid)
        struct.pack_into('<Q', broken, 0, offset)
        malformed.append(broken)
    cycle = bytearray(valid)
    struct.pack_into('<Q', cycle, 12, 1)
    malformed.append(cycle)
    # No complete payload, and no trips at all.
    malformed += [bytes(12), struct.pack('<QI', 2**64 - 1, 0)]
    for number, content in enumerate(malformed):
        bad = root / f'malformed-{number}.kdtrip'
        bad.write_bytes(content)
        run([checker, str(bad), 'validate'], success=False)
    print(f'{len(malformed)} malformed indexes rejected cleanly')
    print('bundled sample', run([checker, sample, '10000']).stdout.strip())
