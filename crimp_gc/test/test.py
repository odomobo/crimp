#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

def get_test_list(executable):
    """Run 'test_crimpgc list' and parse output"""
    result = subprocess.run([executable, "list"], capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error listing tests: {result.stderr}")
        sys.exit(1)

    tests = []
    for line in result.stdout.strip().split('\n'):
        if line:
            parts = line.split()
            if len(parts) >= 3:
                index = int(parts[0])
                name = parts[1]
                expected_code = int(parts[2])
                tests.append((index, name, expected_code))
    return tests

def run_test(executable, index, name, expected_code):
    """Run a single test case"""
    result = subprocess.run([executable, "runCase", str(index)], capture_output=True, text=True)

    if result.returncode == expected_code:
        print(f"[PASS] {name}")
        return True
    else:
        print(f"[FAIL] {name} (expected {expected_code}, got {result.returncode})")
        if result.stderr:
            print(f"  stderr: {result.stderr}")
        return False

def main():
    # Find test executable
    script_dir = Path(__file__).parent
    build_dir = script_dir.parent / "build" / "bin"
    executable = build_dir / "test_crimpgc"

    if not executable.exists():
        print(f"Error: Test executable not found at {executable}")
        sys.exit(1)

    # Get test list
    tests = get_test_list(str(executable))
    print(f"Running {len(tests)} tests...\n")

    # Run each test
    passed = 0
    failed = 0
    for index, name, expected_code in tests:
        if run_test(str(executable), index, name, expected_code):
            passed += 1
        else:
            failed += 1

    # Print summary
    print(f"\n{'='*50}")
    print(f"Tests passed: {passed}/{len(tests)}")
    if failed > 0:
        print(f"Tests failed: {failed}")
        sys.exit(1)
    else:
        print("All tests passed!")
        sys.exit(0)

if __name__ == "__main__":
    main()
