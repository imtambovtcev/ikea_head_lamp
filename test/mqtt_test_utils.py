"""
MQTT Test Utilities for IKEA Head Lamp
Provides helper classes and functions for testing firmware via MQTT
"""

import json
import time
import sys
from typing import Optional, Dict, Any, Callable
from dataclasses import dataclass
from datetime import datetime

try:
    import paho.mqtt.client as mqtt
    from colorama import Fore, Style, init as colorama_init
    colorama_init(autoreset=True)
except ImportError:
    print("❌ Missing required packages. Install with:")
    print("   pip install paho-mqtt colorama")
    sys.exit(1)

try:
    from mqtt_test_config import MQTT_CONFIG
except ImportError:
    print("❌ mqtt_test_config.py not found!")
    print("   Copy mqtt_test_config.py.example to mqtt_test_config.py")
    sys.exit(1)


@dataclass
class TestResult:
    """Result of a test assertion"""
    passed: bool
    message: str
    expected: Any = None
    actual: Any = None


class MQTTTestClient:
    """MQTT client for testing with message capture and assertions"""

    def __init__(self):
        self.config = MQTT_CONFIG
        self.client = mqtt.Client()
        self.messages = []
        self.last_message = {}
        self.connected = False

        # Set up authentication if provided
        if self.config.get("username"):
            self.client.username_pw_set(
                self.config["username"],
                self.config.get("password", "")
            )

        # Set up callbacks
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message

    def _on_connect(self, client, userdata, flags, rc):
        """Callback when connected to broker"""
        if rc == 0:
            self.connected = True
            topic = f"{self.config['device_topic']}/#"
            client.subscribe(topic)
        else:
            print(f"{Fore.RED}❌ Connection failed with code {rc}")

    def _on_message(self, client, userdata, msg):
        """Callback when message received"""
        topic = msg.topic
        payload = msg.payload.decode('utf-8')

        # Store message
        message = {
            'topic': topic,
            'payload': payload,
            'timestamp': datetime.now()
        }
        self.messages.append(message)

        # Try to parse JSON
        try:
            message['json'] = json.loads(payload)
        except:
            message['json'] = None

        # Store by topic suffix
        topic_suffix = topic.replace(f"{self.config['device_topic']}/", "")
        self.last_message[topic_suffix] = message

    def connect(self):
        """Connect to MQTT broker"""
        try:
            self.client.connect(self.config["host"], self.config["port"], 60)
            self.client.loop_start()

            # Wait for connection
            timeout = 5
            start = time.time()
            while not self.connected and (time.time() - start) < timeout:
                time.sleep(0.1)

            if not self.connected:
                raise Exception("Connection timeout")

            return True
        except Exception as e:
            print(f"{Fore.RED}❌ Failed to connect: {e}")
            return False

    def disconnect(self):
        """Disconnect from broker"""
        self.client.loop_stop()
        self.client.disconnect()

    def publish(self, subtopic: str, payload: str):
        """Publish message to device"""
        topic = f"{self.config['device_topic']}/{subtopic}"
        self.client.publish(topic, payload)

    def clear_messages(self):
        """Clear captured messages"""
        self.messages = []
        self.last_message = {}

    def wait_for_message(self, subtopic: str, timeout: float = 2.0) -> Optional[Dict]:
        """Wait for a message on specific subtopic"""
        start = time.time()
        while (time.time() - start) < timeout:
            if subtopic in self.last_message:
                return self.last_message[subtopic]
            time.sleep(0.1)
        return None

    def get_all_topics_received(self) -> list:
        """Get list of all topic suffixes that have received messages"""
        return list(self.last_message.keys())

    def get_recent_messages(self, limit: int = 10) -> list:
        """Get recent messages for debugging"""
        return self.messages[-limit:]

    def assert_json_field(self, subtopic: str, field: str, expected: Any,
                          timeout: float = 2.0) -> TestResult:
        """Assert that a JSON field in a message matches expected value"""
        msg = self.wait_for_message(subtopic, timeout)

        if not msg:
            topics = self.get_all_topics_received()
            error_msg = f"No message received on {subtopic}"
            if topics:
                error_msg += f"\n  {Fore.YELLOW}Topics available: {topics}{Style.RESET_ALL}"
            return TestResult(
                passed=False,
                message=error_msg,
                expected=expected,
                actual=None
            )

        if not msg.get('json'):
            return TestResult(
                passed=False,
                message=f"Message is not JSON: {msg['payload']}",
                expected=expected,
                actual=msg['payload']
            )

        actual = msg['json'].get(field)
        passed = actual == expected

        result_msg = f"{subtopic}.{field}"
        if not passed:
            result_msg += f"\n  {Fore.YELLOW}Full message: {msg['json']}{Style.RESET_ALL}"

        return TestResult(
            passed=passed,
            message=result_msg,
            expected=expected,
            actual=actual
        )

    def assert_animation_running(self, anim_name: str, timeout: float = 2.0) -> TestResult:
        """Assert that specific animation is running"""
        return self.assert_json_field("state/json", "anim", anim_name, timeout)

    def get_config_state(self, timeout: float = 3.0) -> Optional[Dict]:
        """Request and return config state"""
        self.clear_messages()
        self.publish("config/request", "1")
        time.sleep(0.5)  # Give device time to respond
        msg = self.wait_for_message("config/state", timeout)
        if not msg:
            topics = self.get_all_topics_received()
            print(
                f"{Fore.YELLOW}⚠️  No 'config/state' response received{Style.RESET_ALL}")
            print(
                f"{Fore.YELLOW}   Topics received during wait: {topics}{Style.RESET_ALL}")
            if self.messages:
                print(f"{Fore.YELLOW}   Recent messages:{Style.RESET_ALL}")
                for m in self.get_recent_messages(5):
                    topic_short = m['topic'].replace(
                        f"{self.config['device_topic']}/", "")
                    payload_short = m['payload'][:100] + \
                        "..." if len(m['payload']) > 100 else m['payload']
                    print(
                        f"{Fore.YELLOW}     - {topic_short}: {payload_short}{Style.RESET_ALL}")
        return msg['json'] if msg and msg.get('json') else None


def print_header(title: str):
    """Print a formatted test header"""
    print()
    print(f"{Fore.CYAN}{'=' * 70}")
    print(f"{Fore.CYAN}{title:^70}")
    print(f"{Fore.CYAN}{'=' * 70}{Style.RESET_ALL}")
    print()


def print_step(step_num: int, description: str):
    """Print a test step"""
    print(f"{Fore.YELLOW}📋 Step {step_num}: {description}{Style.RESET_ALL}")


def print_result(result: TestResult):
    """Print test result with color coding"""
    if result.passed:
        print(f"{Fore.GREEN}✓ PASS: {result.message}{Style.RESET_ALL}")
        if result.expected is not None:
            print(f"  Expected: {result.expected}")
    else:
        print(f"{Fore.RED}✗ FAIL: {result.message}{Style.RESET_ALL}")
        print(f"  Expected: {Fore.GREEN}{result.expected}{Style.RESET_ALL}")
        print(f"  Actual:   {Fore.RED}{result.actual}{Style.RESET_ALL}")


def print_config(config: Dict, highlight_fields: list = None):
    """Pretty print config with optional field highlighting"""
    print(f"{Fore.CYAN}Config State:{Style.RESET_ALL}")
    for key, value in config.items():
        if highlight_fields and key in highlight_fields:
            print(f"  {Fore.YELLOW}{key}: {value}{Style.RESET_ALL}")
        else:
            print(f"  {key}: {value}")


def run_test_suite(test_name: str, test_func: Callable) -> bool:
    """Run a test suite and return success status"""
    print_header(test_name)

    client = MQTTTestClient()

    print(f"{Fore.CYAN}📡 Connecting to broker...")
    print(f"   Host: {client.config['host']}:{client.config['port']}")
    print(f"   Topic: {client.config['device_topic']}{Style.RESET_ALL}")
    print()

    if not client.connect():
        return False

    print(f"{Fore.GREEN}✓ Connected{Style.RESET_ALL}")
    print()

    try:
        results = test_func(client)

        # Print summary
        passed = sum(1 for r in results if r.passed)
        total = len(results)

        print()
        print(f"{Fore.CYAN}{'=' * 70}")
        if passed == total:
            print(
                f"{Fore.GREEN}✓ ALL TESTS PASSED: {passed}/{total}{Style.RESET_ALL}")
            success = True
        else:
            print(
                f"{Fore.RED}✗ SOME TESTS FAILED: {passed}/{total} passed{Style.RESET_ALL}")
            success = False
        print(f"{Fore.CYAN}{'=' * 70}{Style.RESET_ALL}")

        return success

    finally:
        client.disconnect()
