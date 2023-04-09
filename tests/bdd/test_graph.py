import unittest
from uuid import uuid4

from graph import Node, Graph

class test_node(unittest.TestCase):
    def test_init(self):
        node = Node()
        self.assertIsInstance(node.data, dict)

if __name__ == '__main__':
    unittest.main()
