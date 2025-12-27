#!/usr/bin/env python3
"""
Manual test suite for physical button controls
Prompts user to press buttons and validates MQTT responses
"""

import time
from mqtt_test_utils import (
    MQTTTestClient,
    TestResult,
    print_header,
    print_step,
    run_test_suite,
)
from colorama import Fore, Style


def test_button_controls(client: MQTTTestClient) -> list[TestResult]:
    """Run manual button control tests"""
    results = []

    print(f"{Fore.YELLOW}⚠️  This test requires manual button presses!{Style.RESET_ALL}")
    print()

    # Test 1: Single Click - Power Toggle
    print_step(1, "Test Single Click (Power Toggle)")
    print(
        f"{Fore.CYAN}👉 Please press the button ONCE (single click){Style.RESET_ALL}"
    )
    print("   Expected: Power toggles ON or OFF")
    print("   Waiting 10 seconds for button press...")
    print()

    client.clear_messages()
    start_time = time.time()
    initial_state = None
    final_state = None

    # Wait for state change
    while (time.time() - start_time) < 10:
        msg = client.wait_for_message("state/json", timeout=0.5)
        if msg and msg.get("json"):
            if initial_state is None:
                initial_state = msg["json"].get("pwr")
            else:
                new_pwr = msg["json"].get("pwr")
                if new_pwr != initial_state:
                    final_state = new_pwr
                    break

    if final_state is not None and initial_state is not None:
        result = TestResult(
            passed=final_state != initial_state,
            message="Single click toggles power",
            expected=f"Power changed from {initial_state}",
            actual=f"Power is now {final_state}",
        )
        print(
            f"{Fore.GREEN}✓ Power changed: {initial_state} → {final_state}{Style.RESET_ALL}")
    else:
        result = TestResult(
            passed=False,
            message="Single click toggles power",
            expected="Power state change detected",
            actual="No power state change detected",
        )
        print(f"{Fore.RED}✗ No power state change detected{Style.RESET_ALL}")
        print(f"{Fore.YELLOW}   Did you press the button?{Style.RESET_ALL}")

    results.append(result)
    print()
    time.sleep(2)

    # Test 2: Long Press - Pause/Play Animation
    print_step(2, "Test Long Press (Pause/Play Animation)")
    print(f"{Fore.CYAN}   First, starting an animation...{Style.RESET_ALL}")
    client.publish("cmnd/animation", "rainbow")
    time.sleep(3)

    print(
        f"{Fore.CYAN}👉 Please HOLD the button for ~1 second (long press){Style.RESET_ALL}"
    )
    print("   Expected: Animation pauses")
    print("   Waiting 10 seconds for button press...")
    print()

    client.clear_messages()
    start_time = time.time()
    pause_detected = False

    # Wait for pause state change
    while (time.time() - start_time) < 10:
        msg = client.wait_for_message("state/json", timeout=0.5)
        if msg and msg.get("json"):
            if msg["json"].get("pause") == 1:
                pause_detected = True
                break

    if pause_detected:
        result = TestResult(
            passed=True,
            message="Long press pauses animation",
            expected="pause=1",
            actual="Animation paused",
        )
        print(f"{Fore.GREEN}✓ Animation paused{Style.RESET_ALL}")

        # Now test unpause
        print()
        print(f"{Fore.CYAN}👉 Please HOLD the button again to unpause{Style.RESET_ALL}")
        print("   Waiting 10 seconds...")
        print()

        client.clear_messages()
        start_time = time.time()
        unpause_detected = False

        while (time.time() - start_time) < 10:
            msg = client.wait_for_message("state/json", timeout=0.5)
            if msg and msg.get("json"):
                if msg["json"].get("pause") == 0:
                    unpause_detected = True
                    break

        if unpause_detected:
            print(f"{Fore.GREEN}✓ Animation unpaused{Style.RESET_ALL}")
            result2 = TestResult(
                passed=True,
                message="Long press unpauses animation",
                expected="pause=0",
                actual="Animation playing",
            )
            results.append(result2)
    else:
        result = TestResult(
            passed=False,
            message="Long press pauses animation",
            expected="pause=1",
            actual="No pause detected",
        )
        print(f"{Fore.RED}✗ No pause detected{Style.RESET_ALL}")
        print(
            f"{Fore.YELLOW}   Did you hold the button for ~1 second?{Style.RESET_ALL}")

    results.append(result)
    print()
    time.sleep(2)

    # Test 3: Double Click - Favorite Animation
    print_step(3, "Test Double Click (Favorite Animation)")
    print(
        f"{Fore.CYAN}👉 Please press the button TWICE quickly (double click){Style.RESET_ALL}"
    )
    print("   Expected: Favorite animation starts")
    print("   Waiting 10 seconds for button press...")
    print()

    # First get the favorite animation from config
    config = client.get_config_state(timeout=2)
    expected_anim = "fire"  # default
    if config and "favorite_animation" in config:
        expected_anim = config["favorite_animation"]
        print(
            f"   Favorite animation is: {Fore.YELLOW}{expected_anim}{Style.RESET_ALL}")
        print()

    client.clear_messages()
    start_time = time.time()
    favorite_started = False

    # Wait for favorite animation to start
    while (time.time() - start_time) < 10:
        msg = client.wait_for_message("state/json", timeout=0.5)
        if msg and msg.get("json"):
            anim = msg["json"].get("anim")
            if anim == expected_anim:
                favorite_started = True
                break

    if favorite_started:
        result = TestResult(
            passed=True,
            message="Double click starts favorite animation",
            expected=expected_anim,
            actual=f"{expected_anim} started",
        )
        print(
            f"{Fore.GREEN}✓ Favorite animation '{expected_anim}' started{Style.RESET_ALL}")
    else:
        result = TestResult(
            passed=False,
            message="Double click starts favorite animation",
            expected=expected_anim,
            actual="No animation change detected",
        )
        print(f"{Fore.RED}✗ Favorite animation did not start{Style.RESET_ALL}")
        print(f"{Fore.YELLOW}   Did you double-click the button?{Style.RESET_ALL}")

    results.append(result)
    print()

    # Stop animation for next tests
    client.publish("cmnd/animation", "stop")
    time.sleep(1)

    return results


if __name__ == "__main__":
    print()
    print(f"{Fore.YELLOW}╔════════════════════════════════════════════════════════════════╗")
    print(f"{Fore.YELLOW}║  MANUAL BUTTON TEST - USER INTERACTION REQUIRED              ║")
    print(f"{Fore.YELLOW}║  Please have your device ready and be prepared to press       ║")
    print(f"{Fore.YELLOW}║  the button when prompted!                                    ║")
    print(f"{Fore.YELLOW}╚════════════════════════════════════════════════════════════════╝{Style.RESET_ALL}")
    print()

    input("Press Enter when ready to start button tests...")

    success = run_test_suite(
        "Button Controls Test (Manual)", test_button_controls)
    exit(0 if success else 1)
