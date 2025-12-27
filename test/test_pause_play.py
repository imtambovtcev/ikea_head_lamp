#!/usr/bin/env python3
"""
Test suite for animation pause/play functionality
Tests: pause, unpause, pause toggle
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


def test_pause_play(client: MQTTTestClient) -> list[TestResult]:
    """Run pause/play tests"""
    results = []

    # Reset state
    print_step(0, "Resetting state (power off)")
    client.clear_messages()
    client.publish("cmnd/power", "off")
    time.sleep(1.5)
    print()

    # Start an animation first
    print_step(1, "Starting rainbow animation for testing")
    client.publish("cmnd/animation", "rainbow")
    time.sleep(2)

    result = client.assert_animation_running("rainbow")
    results.append(result)
    print_result(result)
    print()

    # Test 2: Pause
    print_step(2, "Pause the animation")
    client.clear_messages()
    client.publish("cmnd/pause", "true")
    time.sleep(1)

    result = client.assert_json_field("state/json", "pause", 1)
    results.append(result)
    print_result(result)
    print()
    time.sleep(2)

    # Test 3: Unpause
    print_step(3, "Unpause the animation")
    client.clear_messages()
    client.publish("cmnd/pause", "false")
    time.sleep(1)

    result = client.assert_json_field("state/json", "pause", 0)
    results.append(result)
    print_result(result)
    print()
    time.sleep(2)

    # Test 4: Toggle pause (should pause)
    print_step(4, "Toggle pause (should pause)")
    client.clear_messages()
    client.publish("cmnd/pause", "toggle")
    time.sleep(1)

    result = client.assert_json_field("state/json", "pause", 1)
    results.append(result)
    print_result(result)
    print()
    time.sleep(2)

    # Test 5: Toggle pause again (should unpause)
    print_step(5, "Toggle pause again (should unpause)")
    client.clear_messages()
    client.publish("cmnd/pause", "toggle")
    time.sleep(1)

    result = client.assert_json_field("state/json", "pause", 0)
    results.append(result)
    print_result(result)
    print()
    time.sleep(2)

    # Test 6: Pause with "1"
    print_step(6, 'Pause with "1"')
    client.clear_messages()
    client.publish("cmnd/pause", "1")
    time.sleep(1)

    result = client.assert_json_field("state/json", "pause", 1)
    results.append(result)
    print_result(result)
    print()
    time.sleep(1)

    # Test 7: Unpause with "0"
    print_step(7, 'Unpause with "0"')
    client.clear_messages()
    client.publish("cmnd/pause", "0")
    time.sleep(1)

    result = client.assert_json_field("state/json", "pause", 0)
    results.append(result)
    print_result(result)
    print()

    # Clean up
    client.publish("cmnd/animation", "stop")
    time.sleep(1)

    return results


if __name__ == "__main__":
    success = run_test_suite("Animation Pause/Play Test", test_pause_play)
    exit(0 if success else 1)
