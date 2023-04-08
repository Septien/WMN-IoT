import unittest
from wmn_bdd import WMN
import paho.mqtt.client as mqtt

class test_wmn(unittest.TestCase):
    def test_init(self):
      wmn = WMN()

      self.assertIsInstance(wmn.nodes, mqtt.Client)
      self.assertTrue(wmn.nodes.is_connected())

if __name__ == '__main__':
    unittest.main()
