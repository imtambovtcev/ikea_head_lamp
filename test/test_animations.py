#!/usr/bin/env python3
"""
Test script for all animations
Validates each animation starts and runs correctly
"""

import time
from mqtt_test_utils import (
    MQTTTestClient, TestResult,
    print_header, print_step, print_result,
    run_test_suite
)


def test_animations(client: MQTTTestClient) -> list[TestResult]:
    """Run animation tests"""
    results = []

    # Reset state
    print_step(0, "Resetting state (power off)")
    client.clear_messages()
    client.publish("cmnd/power", "off")
    time.sleep(1.5)
    print()

    # Test 1: Fire animation
    print_step(1, "Fire animation (intensity=80, speed=6)")
    client.clear_messages()
    client.publish("cmnd/animation", "fire:intensity=80,speed=6")
    time.sleep(2)

    result = client.assert_animation_running("fire")
    results.append(result)
    print_result(result)
    print()
    time.sleep(8)

    # Test 2: Ocean animation
    print_step(2, "Ocean animation (speed=6)")
    client.clear_messages()
    client.publish("cmnd/animation", "ocean:speed=6")
    time.sleep(2)

    result = client.assert_animation_running("ocean")
    results.append(result)
    print_result(result)
    print()
    time.sleep(8)

    # Test 3: Breathe animation
    print_step(3, "Breathe animation (duration=3s, pink color)")
    client.clear_messages()
    client.publish("cmnd/animation", "breathe:duration=3,color=255,50,150")
    time.sleep(2)

    result = client.assert_animation_running("breathe")
    results.append(result)
    print_result(result)

    # Check color parameters
    msg = client.wait_for_message("state/json", timeout=1)
    if msg and msg.get('json'):
        state = msg['json']
        final_rgb = state.get('final_rgb', [])
        result = TestResult(
            passed=final_rgb == [255, 50, 150],
            message="Breathe color parameters",
            expected=[255, 50, 150],
            actual=final_rgb
        )
        results.append(result)
        print_result(result)

    print()
    time.sleep(8)

    # Test 4: Rainbow animation
    print_step(4, "Rainbow animation")
    client.clear_messages()
    client.publish("cmnd/animation", "rainbow")
    time.sleep(2)

    result = client.assert_animation_running("rainbow")
    results.append(result)
    print_result(result)
    print()
    time.sleep(6)

    # Test 5: Stop animation
    print_step(5, "Stop animation")
    client.clear_messages()
    client.publish("cmnd/animation", "stop")
    time.sleep(1)

    result = client.assert_json_field("state/json", "anim", "", timeout=2)
    results.append(result)
    print_result(result)
    print()

    return results


if __name__ == "__main__":
    success = run_test_suite("Animation Test Suite", test_animations)
    exit(0 if success else 1)
