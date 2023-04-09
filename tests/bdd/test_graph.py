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

    def test_add_edge_twice(self):
        node_id1 = uuid4().hex
        data1 = {'data': 1, 'data2': [1, 2, 3, 4]}
        self.graph.add_vertex(node_id1, data1)
        node_id2 = uuid4().hex
        data2 = {'data': 2, 'data2': [1, 2, 3, 4, 5]}
        self.graph.add_vertex(node_id2, data2)

        self.graph.add_edge(node_id1, node_id2)
        with self.assertRaises(ValueError):
            self.graph.add_edge(node_id1, node_id2)

        with self.assertRaises(ValueError):
            self.graph.add_edge(node_id2, node_id1)

    def test_add_edge_same_node(self):
        node_id1 = uuid4().hex
        data1 = {'data': 1, 'data2': [1, 2, 3, 4]}
        self.graph.add_vertex(node_id1, data1)

        with self.assertRaises(ValueError):
            self.graph.add_edge(node_id1, node_id1)

    def test_add_several_neighbors_one_node(self):
        data = {'data': 1, 'data2': [1, 2, 3, 4]}
        nodes_id = []
        for i in range(6):
            nodes_id.append(uuid4().hex)
            self.graph.add_vertex(nodes_id[-1], data)

        for i in range(1, 6):
            self.graph.add_edge(nodes_id[0], nodes_id[i])

        for i in range(1, 6):
            self.assertEqual(self.graph.adjList[0][i - 1], i)

    def test_add_several_neighbors_several_nodes(self):
        data = {'data': 1, 'data2': [1, 2, 3, 4]}
        nodes_id = []
        for i in range(10):
            nodes_id.append(uuid4().hex)
            self.graph.add_vertex(nodes_id[-1], data)

        for i in range(10):
            _id = nodes_id[i]
            for j in range(i + 1, i + 4):
                self.graph.add_edge(_id, nodes_id[j%10])

        #for i in range(10):
        #    _id = nodes_id[i]
        #    neighbors = self.graph.adjList[self.graph.index_map[_id]][0:3]
        #    _neighbors = [j for j in range(i + 1, i + 4)]
        #    with self.subTest(i=i):
        #        self.assertEqual(neighbors, _neighbors)


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
