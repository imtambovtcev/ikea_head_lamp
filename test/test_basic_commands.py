#!/usr/bin/env python3
"""
Comprehensive test suite for basic MQTT commands
Tests: power, brightness, color, static mode
"""

import time
from mqtt_test_utils import (
    MQTTTestClient,
    TestResult,
    print_header,
    print_step,
    print_result,
    run_test_suite,
)


def test_basic_commands(client: MQTTTestClient) -> list[TestResult]:
    """Run basic command tests"""
    results = []

    # Reset state
    print_step(0, "Resetting state (power off)")
    client.clear_messages()
    client.publish("cmnd/power", "off")
    time.sleep(1.5)
    print()

    # Test 1: Power ON
    print_step(1, "Power ON command")
    client.clear_messages()
    client.publish("cmnd/power", "on")
    time.sleep(1)

    result = client.assert_json_field("state/json", "pwr", 1)
    results.append(result)
    print_result(result)
    print()

    # Test 2: Power OFF
    print_step(2, "Power OFF command")
    client.clear_messages()
    client.publish("cmnd/power", "off")
    time.sleep(1)

    result = client.assert_json_field("state/json", "pwr", 0)
    results.append(result)
    print_result(result)
    print()

    # Test 3: Power Toggle
    print_step(3, "Power Toggle (should turn ON)")
    client.clear_messages()
    client.publish("cmnd/power", "toggle")
    time.sleep(1)

    result = client.assert_json_field("state/json", "pwr", 1)
    results.append(result)
    print_result(result)
    print()

    # Test 4: Set Brightness
    print_step(4, "Set brightness to 50%")
    client.clear_messages()
    client.publish("cmnd/brightness", "50")
    time.sleep(1)

    result = client.assert_json_field("state/json", "bri", 50)
    results.append(result)
    print_result(result)
    print()

    # Test 5: Set Color (Warm White)
    print_step(5, "Set color to warm white (255,147,41)")
    client.clear_messages()
    client.publish("cmnd/color", "255,147,41")
    time.sleep(1)

    msg = client.wait_for_message("state/json", timeout=2)
    if msg and msg.get("json"):
        rgb = msg["json"].get("rgb", [])
        result = TestResult(
            passed=rgb == [255, 147, 41],
            message="Color RGB values",
            expected=[255, 147, 41],
            actual=rgb,
        )
    else:
        result = TestResult(False, "No state message received", [
                            255, 147, 41], None)

    results.append(result)
    print_result(result)
    print()

    # Test 6: Set Color (Blue)
    print_step(6, "Set color to blue (0,100,255)")
    client.clear_messages()
    client.publish("cmnd/color", "0,100,255")
    time.sleep(1)

    msg = client.wait_for_message("state/json", timeout=2)
    if msg and msg.get("json"):
        rgb = msg["json"].get("rgb", [])
        result = TestResult(
            passed=rgb == [0, 100, 255],
            message="Color RGB values",
            expected=[0, 100, 255],
            actual=rgb,
        )
    else:
        result = TestResult(False, "No state message received", [
                            0, 100, 255], None)

    results.append(result)
    print_result(result)
    print()

    # Test 7: Set Brightness to 100%
    print_step(7, "Set brightness to 100%")
    client.clear_messages()
    client.publish("cmnd/brightness", "100")
    time.sleep(1)

    result = client.assert_json_field("state/json", "bri", 100)
    results.append(result)
    print_result(result)
    print()

    # Test 8: Set Brightness to 10%
    print_step(8, "Set brightness to 10%")
    client.clear_messages()
    client.publish("cmnd/brightness", "10")
    time.sleep(1)

    result = client.assert_json_field("state/json", "bri", 10)
    results.append(result)
    print_result(result)
    print()

    # Test 9: Apply Defaults
    print_step(9, "Apply default settings")
    client.clear_messages()
    client.publish("cmnd/apply_defaults", "1")
    time.sleep(1)

    result = client.assert_json_field("state/json", "pwr", 1)
    results.append(result)
    print_result(result)
    print()

    return results


if __name__ == "__main__":
    success = run_test_suite("Basic MQTT Commands Test", test_basic_commands)
    exit(0 if success else 1)
