import unittest
from uuid import uuid4

from graph import Node, Graph

class test_node(unittest.TestCase):
    def test_init(self):
        node = Node()
        self.assertIsInstance(node.data, dict)

class test_graph(unittest.TestCase):
    def setUp(self) -> None:
        V = 10
        self.graph = Graph(V)

    def tearDown(self) -> None:
        del self.graph

    def test_init(self):
        V = 100
        g = Graph(V)
        self.assertEqual(len(g.nodes), V)
        self.assertIsInstance(g.index_map, dict)
        self.assertEqual(len(g.adjList), V)
        self.assertEqual(g.last_index, 0)

    def test_add_vertex(self):
        node_id = uuid4().hex
        data = {'data': 1, 'data2': [1, 2, 3, 4]}
        self.graph.add_vertex(node_id, data)
        self.assertEqual(self.graph.last_index, 1)
        self.assertEqual(self.graph.index_map[node_id], 0)
        data["node id"] = node_id
        self.assertEqual(self.graph.nodes[0].data, data)
        self.assertIn("node id", self.graph.nodes[0].data)


if __name__ == '__main__':
    unittest.main()
