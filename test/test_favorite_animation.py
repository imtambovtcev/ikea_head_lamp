#!/usr/bin/env python3
"""
Test script for favorite animation feature
Validates:
- Default favorite animation (fire)
- Changing favorite via MQTT
- Saving to NVS
- Triggering favorite with MQTT command
- Custom parameters
"""

import time
from mqtt_test_utils import (
    MQTTTestClient, TestResult,
    print_header, print_step, print_result, print_config,
    run_test_suite
)


def test_favorite_animation(client: MQTTTestClient) -> list[TestResult]:
    """Run favorite animation tests"""
    results = []

    # Reset state
    print_step(0, "Resetting state (power off)")
    client.clear_messages()
    client.publish("cmnd/power", "off")
    time.sleep(1.5)
    print()

    # Step 1: Get initial config with default favorite
    print_step(1, "Request config to see default favorite animation")
    config = client.get_config_state(timeout=3)

    if config:
        print_config(config, highlight_fields=[
                     'favorite_animation', 'favorite_params'])

        result = TestResult(
            passed='favorite_animation' in config,
            message="Config contains favorite_animation field",
            expected="'favorite_animation' field present",
            actual=config.keys() if config else None
        )
        results.append(result)
        print_result(result)

        if 'favorite_animation' in config:
            result = TestResult(
                passed=config['favorite_animation'] == 'fire',
                message="Default favorite is 'fire'",
                expected='fire',
                actual=config.get('favorite_animation')
            )
            results.append(result)
            print_result(result)

            result = TestResult(
                passed=config.get('favorite_params', [None])[0] == 70,
                message="Default fire intensity is 70",
                expected=70,
                actual=config.get('favorite_params', [None])[0]
            )
            results.append(result)
            print_result(result)
    else:
        result = TestResult(False, "Failed to get config", None, None)
        results.append(result)
        print_result(result)

    print()
    time.sleep(1)

    # Step 2: Start favorite animation (should be fire)
    print_step(2, "Start favorite animation (should trigger fire)")
    client.clear_messages()
    client.publish("cmnd/animation", "favorite")
    time.sleep(2)

    result = client.assert_animation_running("fire")
    results.append(result)
    print_result(result)

    print()
    time.sleep(4)

    # Step 3: Change favorite to rainbow
    print_step(3, "Change favorite animation to rainbow")
    client.clear_messages()
    client.publish("config/favorite_animation/set", "rainbow")
    time.sleep(1)

    # Step 4: Verify config updated
    print_step(4, "Verify config shows rainbow as favorite")
    config = client.get_config_state(timeout=3)

    if config:
        print_config(config, highlight_fields=['favorite_animation'])

        result = TestResult(
            passed=config.get('favorite_animation') == 'rainbow',
            message="Favorite changed to 'rainbow'",
            expected='rainbow',
            actual=config.get('favorite_animation')
        )
        results.append(result)
        print_result(result)
    else:
        result = TestResult(False, "Failed to get config", 'rainbow', None)
        results.append(result)
        print_result(result)

    print()
    time.sleep(1)

    # Step 5: Save config
    print_step(5, "Save config to NVS flash")
    client.publish("config/save", "1")
    time.sleep(2)

    # Step 6: Start favorite again (should be rainbow now)
    print_step(6, "Start favorite animation (should now trigger rainbow)")
    client.clear_messages()
    client.publish("cmnd/animation", "favorite")
    time.sleep(2)

    result = client.assert_animation_running("rainbow")
    results.append(result)
    print_result(result)

    print()
    time.sleep(5)

    # Step 7: Change to ocean with parameters
    print_step(
        7, "Change favorite to ocean with custom params (speed=8, brightness=50)")
    client.clear_messages()
    client.publish("config/favorite_animation/set",
                   "ocean:speed=8,brightness=50")
    time.sleep(1)

    # Step 8: Verify parameters stored
    print_step(8, "Verify config shows ocean with parameters")
    config = client.get_config_state(timeout=3)

    if config:
        print_config(config, highlight_fields=[
                     'favorite_animation', 'favorite_params'])

        result = TestResult(
            passed=config.get('favorite_animation') == 'ocean',
            message="Favorite changed to 'ocean'",
            expected='ocean',
            actual=config.get('favorite_animation')
        )
        results.append(result)
        print_result(result)

        # Ocean stores brightness in param1, speed in param2
        params = config.get('favorite_params', [])
        if len(params) >= 2:
            result = TestResult(
                passed=params[0] == 50,  # brightness
                message="Ocean brightness parameter",
                expected=50,
                actual=params[0]
            )
            results.append(result)
            print_result(result)

            result = TestResult(
                passed=params[1] == 8,  # speed
                message="Ocean speed parameter",
                expected=8,
                actual=params[1]
            )
            results.append(result)
            print_result(result)

    print()
    time.sleep(1)

    # Step 9: Save config
    print_step(9, "Save config")
    client.publish("config/save", "1")
    time.sleep(2)

    # Step 10: Start favorite (should be ocean)
    print_step(10, "Start favorite animation (should trigger ocean)")
    client.clear_messages()
    client.publish("cmnd/animation", "favorite")
    time.sleep(2)

    result = client.assert_animation_running("ocean")
    results.append(result)
    print_result(result)

    time.sleep(4)

    return results


if __name__ == "__main__":
    success = run_test_suite(
        "Favorite Animation Feature Test", test_favorite_animation)
    exit(0 if success else 1)
