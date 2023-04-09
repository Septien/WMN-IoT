import unittest
from uuid import uuid4

from graph import Node, Graph

class test_node(unittest.TestCase):
    def test_init(self):
        node = Node()
        self.assertIsInstance(node.data, dict)

class test_graph(unittest.TestCase):
    def test_init(self):
        V = 100
        g = Graph(V)
        self.assertEqual(len(g.nodes), V)
        self.assertIsInstance(g.index_map, dict)
        self.assertEqual(len(g.adjList), V)
        self.assertEqual(g.last_index, 0)


if __name__ == '__main__':
    unittest.main()
