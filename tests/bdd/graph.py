"""
It contains both the data structure for the nodes, and the class that implements a graph.
"""
import copy

class Node:
    data = {}

class Graph:
    def __init__(self, V : int = 1):
        self.last_index = 0
        self.nodes = [Node()] * V
        self.index_map = {}
        self.adjList = [[] for _ in range(V)]

    def add_vertex(self, node_id : int = 0, data : dict = {}) -> None:
        self.index_map[node_id] = self.last_index
        self.nodes[self.last_index].data = copy.deepcopy(data)
        self.nodes[self.last_index].data["node id"] = node_id
        self.last_index += 1
