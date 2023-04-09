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

    def test_add_edge(self):
        node_id1 = uuid4().hex
        data1 = {'data': 1, 'data2': [1, 2, 3, 4]}
        self.graph.add_vertex(node_id1, data1)
        node_id2 = uuid4().hex
        data2 = {'data': 2, 'data2': [1, 2, 3, 4, 5]}
        self.graph.add_vertex(node_id2, data2)

        self.graph.add_edge(node_id1, node_id2)
        index1 = self.graph.index_map[node_id1]
        index2 = self.graph.index_map[node_id2]
        self.assertEqual(self.graph.adjList[index2], [index1])
        self.assertEqual(self.graph.adjList[index1], [index2])

    def test_add_edge_no_existing_id2(self):
        node_id1 = uuid4().hex
        data1 = {'data': 1, 'data2': [1, 2, 3, 4]}
        self.graph.add_vertex(node_id1, data1)
        node_id2 = uuid4().hex

        with self.assertRaises(ValueError):
            self.graph.add_edge(node_id1, node_id2)

    def test_add_edge_no_existing_id1(self):
        node_id1 = uuid4().hex
        data1 = {'data': 1, 'data2': [1, 2, 3, 4]}
        self.graph.add_vertex(node_id1, data1)
        node_id2 = uuid4().hex

        with self.assertRaises(ValueError):
            self.graph.add_edge(node_id2, node_id1)

    def test_get_data(self):
        node_id1 = uuid4().hex
        data1 = {'data': 1, 'data2': [1, 2, 3, 4]}
        self.graph.add_vertex(node_id1, data1)
        data1["node id"] = node_id1

        data = self.graph.get_node_data(node_id1)
        self.assertEqual(data, data1)

    def test_get_data_invalid_id(self):
        node_id1 = uuid4().hex
        data1 = {'data': 1, 'data2': [1, 2, 3, 4]}
        self.graph.add_vertex(node_id1, data1)
        data1["node id"] = node_id1

        _id = uuid4().hex
        with self.assertRaises(ValueError):
            data = self.graph.get_node_data(_id)

if __name__ == '__main__':
    unittest.main()
