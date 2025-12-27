#!/usr/bin/env python3
"""
Master test runner - runs all tests in sequence
"""

import sys
import subprocess
from colorama import Fore, Style, init as colorama_init

colorama_init(autoreset=True)

# List of all test modules
TEST_MODULES = [
    ("Basic Commands", "test_basic_commands.py"),
    ("Configuration", "test_configuration.py"),
    ("Animations", "test_animations.py"),
    ("Pause/Play", "test_pause_play.py"),
    ("Favorite Animation", "test_favorite_animation.py"),
    ("Button Controls (Manual)", "test_button_controls.py"),
]


def run_all_tests():
    """Run all test modules and report results"""
    print(f"{Fore.CYAN}{'=' * 70}")
    print(f"{Fore.CYAN}                    IKEA HEAD LAMP TEST SUITE")
    print(f"{Fore.CYAN}{'=' * 70}{Style.RESET_ALL}")
    print()
    print(f"Running {len(TEST_MODULES)} test suites...")
    print()

    results = []

    for i, (name, module) in enumerate(TEST_MODULES, 1):
        print(
            f"{Fore.YELLOW}[{i}/{len(TEST_MODULES)}] Running: {name}{Style.RESET_ALL}")
        print()

        # Run test module
        try:
            result = subprocess.run(
                [sys.executable, module],
                cwd=".",
                capture_output=False,
                text=True,
            )
            success = result.returncode == 0
            results.append((name, success))
        except Exception as e:
            print(f"{Fore.RED}Error running {name}: {e}{Style.RESET_ALL}")
            results.append((name, False))

        print()
        print(f"{Fore.CYAN}{'-' * 70}{Style.RESET_ALL}")
        print()

    # Print summary
    print()
    print(f"{Fore.CYAN}{'=' * 70}")
    print(f"{Fore.CYAN}                        TEST SUMMARY")
    print(f"{Fore.CYAN}{'=' * 70}{Style.RESET_ALL}")
    print()

    passed = sum(1 for _, success in results if success)
    total = len(results)

    for name, success in results:
        if success:
            print(f"  {Fore.GREEN}✓ PASS{Style.RESET_ALL} - {name}")
        else:
            print(f"  {Fore.RED}✗ FAIL{Style.RESET_ALL} - {name}")

    print()
    print(f"{Fore.CYAN}{'-' * 70}{Style.RESET_ALL}")

    if passed == total:
        print(f"{Fore.GREEN}✓ ALL TESTS PASSED: {passed}/{total}{Style.RESET_ALL}")
        return True
    else:
        print(
            f"{Fore.RED}✗ SOME TESTS FAILED: {passed}/{total} passed{Style.RESET_ALL}")
        return False


if __name__ == "__main__":
    print()
    print(f"{Fore.YELLOW}⚠️  Note: Button Controls test requires manual interaction!{Style.RESET_ALL}")
    print()
    input("Press Enter to start all tests...")
    print()

    success = run_all_tests()
    exit(0 if success else 1)
