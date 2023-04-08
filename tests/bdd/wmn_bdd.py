"""
This script implements a class for simulating the WMN, for making Behavioral tests.
"""
import paho.mqtt.client as mqtt
import time

class WMN:
    def __init__(self):
        def on_connect(client, userdata, flags, rc):
            if rc == 0:
                print("Connected, OK Returned rc = ", rc)
            else:
                print("Bad connection, Returned rc = ", rc)
        self.nodes = mqtt.Client('nodes')
        self.nodes.on_connect = on_connect
        self.nodes.connect('localhost')
        self.nodes.loop_start()

        time.sleep(1)

        self.nodes.subscribe('/nodes')
