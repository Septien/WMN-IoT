import unittest

from wmn_bdd import WMN_Interface

import paho.mqtt.client as mqtt
from queue import Queue
import time

class test_wmn(unittest.TestCase):
    def setUp(self) -> None:
        def on_message(client, userdata, message):
            msg = message.payload.decode('utf-8')
            topic = message.topic
            if [msg, topic] != userdata:
                raise ValueError("Incorrect message")
        #
        self.wmn = WMN_Interface()
        self.client = mqtt.Client('test')
        self.client.connect('localhost')
        self.client.on_message = on_message
        self.client.loop_start()

        while not self.client.is_connected(): continue
        time.sleep(0.5)

    def tearDown(self) -> None:
        self.wmn.disconnect()
        self.client.loop_stop()
        self.client.disconnect()

    def test_init(self):
        wmn = WMN_Interface()

        self.assertIsInstance(wmn.nodes, mqtt.Client)
        self.assertTrue(wmn.nodes.is_connected())
        self.assertIsInstance(wmn.to_receive, Queue)
        self.assertIsNotNone(wmn.topics)
        self.assertListEqual(wmn.topics, ['/nodes'])
        self.assertTrue(self.wmn.nodes.is_connected())
        wmn.disconnect()

    def test_on_message(self):
        m = 'Test message'
        self.client.publish('/nodes', m)
        time.sleep(0.5)

        self.assertFalse(self.wmn.to_receive.empty())
        message = self.wmn.to_receive.get()
        self.assertListEqual(message, [m, '/nodes'])

    def test_on_message_multiple_msgs(self):
        m = 'Testing'
        topic = '/nodes'
        for _ in range(100):
            self.client.publish(topic, m)
        time.sleep(0.5)
        self.assertFalse(self.wmn.to_receive.empty())
        for _ in range(100):
            msg = self.wmn.to_receive.get()
            self.assertListEqual(msg, [m, topic])

    def test_publish_to_send_none(self):
        with self.assertRaises(ValueError):
            self.wmn.message = None

    def test_publish_to_send_len_no_2(self):
        with self.assertRaises(ValueError):
            self.wmn.message = ['topic']
        
        with self.assertRaises(ValueError):
            self.wmn.message = ['topic', 'topic', 'topic']

    def test_publish_no_topic(self):
        msg = 'Test message'
        topic = '/no/topic'

        with self.assertRaises(ValueError):
            self.wmn.message = [msg, topic]

    def test_publish(self):
        msg = 'Test message'
        topic = '/nodes'
        self.client.user_data_set([msg, topic])
        self.client.subscribe(topic)
        self.wmn.message = [msg, topic]
        time.sleep(1)
        recv = self.wmn.to_receive.get()
        self.assertListEqual(recv, [msg, topic])
        self.client.unsubscribe(topic=topic)

    def test_message_none(self):
        msg = self.wmn.message
        self.assertIsNone(msg)

    def test_message_recv(self):
        msg = 'Test message'
        topic = '/nodes'
        self.wmn.message = [msg, topic]
        time.sleep(0.5)

        m = self.wmn.message
        self.assertEqual(m, [msg, topic])

    def test_subscribe_none(self):
        with self.assertRaises(ValueError):
            self.wmn.subscribe(None)
        
        with self.assertRaises(ValueError):
            self.wmn.subscribe('')

    def test_subscribe_one_topic(self):
        topic = '/nodes/1'
        self.wmn.subscribe(topic)
        self.assertEqual(self.wmn.topics[-1], topic)

    def test_subscribe_multiple_topics(self):
        base_topic = '/nodes/'
        topics = []
        for i in range(100):
            topics.append((base_topic + str(i), 1))
        self.wmn.subscribe(topics)
        for i in range(len(topics)):
            self.assertEqual(topics[i][0], self.wmn.topics[i + 1])

    def test_disconnect(self):
        self.assertTrue(self.wmn.nodes.is_connected())
        self.wmn.disconnect()
        self.assertEqual(len(self.wmn.topics), 0)
        self.assertFalse(self.wmn.nodes.is_connected())
        self.assertFalse(self.wmn.nodes.is_connected())

if __name__ == '__main__':
    unittest.main()
