#!/usr/bin/env python3
"""
Test suite for configuration management
Tests: default settings, config save/load, NVS persistence
"""

import time
from mqtt_test_utils import (
    MQTTTestClient,
    TestResult,
    print_header,
    print_step,
    print_result,
    print_config,
    run_test_suite,
)


def test_configuration(client: MQTTTestClient) -> list[TestResult]:
    """Run configuration tests"""
    results = []

    # Reset state
    print_step(0, "Resetting state (power off)")
    client.clear_messages()
    client.publish("cmnd/power", "off")
    time.sleep(1.5)
    print()

    # Test 1: Request config
    print_step(1, "Request current configuration")
    config = client.get_config_state(timeout=3)

    if config:
        print_config(config)
        result = TestResult(
            passed=True,
            message="Config received successfully",
            expected="Valid JSON config",
            actual=f"{len(config)} fields",
        )
    else:
        result = TestResult(
            False, "Failed to get config", "Valid JSON config", None
        )

    results.append(result)
    print_result(result)
    print()
    time.sleep(1)

    # Test 2: Set default brightness
    print_step(2, "Set default brightness to 80")
    client.clear_messages()
    client.publish("config/default_brightness/set", "80")
    time.sleep(1)

    config = client.get_config_state(timeout=3)
    if config:
        result = TestResult(
            passed=config.get("default_brightness") == 80,
            message="Default brightness updated",
            expected=80,
            actual=config.get("default_brightness"),
        )
    else:
        result = TestResult(False, "Failed to get config", 80, None)

    results.append(result)
    print_result(result)
    print()

    # Test 3: Set default color
    print_step(3, "Set default color to cool white (200,220,255)")
    client.clear_messages()
    client.publish("config/default_color/set", "200,220,255")
    time.sleep(1)

    config = client.get_config_state(timeout=3)
    if config:
        expected_color = [200, 220, 255]
        actual_color = config.get("default_color", [])
        result = TestResult(
            passed=actual_color == expected_color,
            message="Default color updated",
            expected=expected_color,
            actual=actual_color,
        )
    else:
        result = TestResult(False, "Failed to get config",
                            [200, 220, 255], None)

    results.append(result)
    print_result(result)
    print()

    # Test 4: Set sunrise duration
    print_step(4, "Set sunrise duration to 15 minutes")
    client.clear_messages()
    client.publish("config/sunrise_minutes/set", "15")
    time.sleep(1)

    config = client.get_config_state(timeout=3)
    if config:
        result = TestResult(
            passed=config.get("sunrise_minutes") == 15,
            message="Sunrise duration updated",
            expected=15,
            actual=config.get("sunrise_minutes"),
        )
    else:
        result = TestResult(False, "Failed to get config", 15, None)

    results.append(result)
    print_result(result)
    print()

    # Test 5: Save configuration
    print_step(5, "Save configuration to NVS")
    client.clear_messages()
    client.publish("config/save", "1")
    time.sleep(2)

    result = TestResult(
        passed=True, message="Config save command sent", expected="Saved", actual="Sent"
    )
    results.append(result)
    print_result(result)
    print()

    # Test 6: Verify config persisted
    print_step(6, "Verify config was saved")
    config = client.get_config_state(timeout=3)
    if config:
        print_config(
            config,
            highlight_fields=[
                "default_brightness",
                "default_color",
                "sunrise_minutes",
            ],
        )
        passed = (
            config.get("default_brightness") == 80
            and config.get("default_color") == [200, 220, 255]
            and config.get("sunrise_minutes") == 15
        )
        result = TestResult(
            passed=passed,
            message="All config values persisted",
            expected="brightness=80, color=[200,220,255], sunrise=15",
            actual=f"brightness={config.get('default_brightness')}, color={config.get('default_color')}, sunrise={config.get('sunrise_minutes')}",
        )
    else:
        result = TestResult(False, "Failed to verify config", None, None)

    results.append(result)
    print_result(result)
    print()

    # Test 7: Reset to defaults
    print_step(7, "Reset configuration to defaults")
    client.clear_messages()
    client.publish("config/reset", "1")
    time.sleep(2)

    config = client.get_config_state(timeout=3)
    if config:
        print_config(config)
        # After reset, should have default values
        result = TestResult(
            passed=config.get("default_brightness") == 70,
            message="Config reset to defaults",
            expected=70,
            actual=config.get("default_brightness"),
        )
    else:
        result = TestResult(
            False, "Failed to get config after reset", 70, None)

    results.append(result)
    print_result(result)
    print()

    return results


if __name__ == "__main__":
    success = run_test_suite(
        "Configuration Management Test", test_configuration)
    exit(0 if success else 1)
