"""
Implements a class that simulates the physical interface among the nodes on the network.
This class acts as the 'central hub' for processing and distributing the messages in the
network. It works as follows:

It connects to a Mosquitto broker in order to receive messages from the nodes present in
the network. Each node has its own topic which uses to publish and receive messages from
the network. This class hears on each topic, and stores the message received for
further processing by the upper class, which simulates the network. It also publishes
or sends the messages to the nodes using their corresponding topics.
"""
import paho.mqtt.client as mqtt
from queue import Queue
import time
from typing import Any

class WMN_Interface:
    """
    Class for simulating the physical network's interface. It has the following members:
        -nodes: the MQTT client for receiving/sending the messages.
        -topics: a list of topics to which the client is connected.
        -to_receive: a list of messages received from the broker.
    """
    def __init__(self):
        def on_connect(client, userdata, flags, rc):
            if rc != 0:
                print("Bad connection, Returned rc = ", rc)
                client.is_connected = True
        # Create client and connect to broker
        self.nodes = mqtt.Client('nodes')
        self.nodes.on_connect = on_connect
        self.nodes.connect('localhost')
        self.nodes.on_message = self.on_message
        self.nodes.loop_start()

        while not self.nodes.is_connected(): continue;

        self.nodes.subscribe('/nodes')
        self.topics = ['/nodes']
        self.to_receive = Queue()

    def on_message(self, client, userdata, message) -> None:
        # Store the message for further processing
        self.to_receive.put([message.payload.decode('utf-8'), message.topic])

    def on_disconnect(self, client, userdata, rc) -> None:
        if rc == 0:
            self.nodes.loop_stop()

    @property
    def message(self) -> list:
        """
        Retrieves the last message received.
        """
        if self.to_receive.empty():
            return None
        return self.to_receive.get()
    
    @message.setter
    def message(self, to_send : list = []) -> None:
        """
        Publish a new message. to_send is a list with the following format:
            -to_send[0]: contains the message to send.
            -to_send[1]: contains the message's topic.
        """
        if to_send is None or len(to_send) != 2:
            raise ValueError("Must pass topic and message")
        if to_send[1] not in self.topics:
            raise ValueError("Topic doesn't exists.")

        msg_info = self.nodes.publish(topic=to_send[1], payload=to_send[0])
        msg_info.wait_for_publish(timeout=1)
    
    def subscribe(self, topic : Any) -> None:
        if topic is None or len(topic) == 0:
            raise ValueError("No topic received")
        self.nodes.subscribe(topic)
        if type(topic) is list:
            for t in topic:
                self.topics.append(t[0])
        else:
            self.topics.append(topic)

    def disconnect(self) -> None:
        for topic in self.topics:
            self.nodes.unsubscribe(topic)
        del self.topics
        self.topics = []
        self.nodes.disconnect()
