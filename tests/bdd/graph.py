"""
It contains both the data structure for the nodes, and the class that implements a graph.
"""
import copy

def verify_key(map, key):
    try:
        index = map[key]
    except:
        string = 'Node with id {} not existing'.format(key)
        raise ValueError(string)
    else:
        return index

class Graph:
    """
        -nodes: Contain the node's data.
        -index_map: Maps from the index to the node's UUID.
    """
    def __init__(self, V : int = 1):
        self.last_index = 0
        self.nodes = [dict() for _ in range(V)]
        self.index_map = {}
        self.adjList = [[] for _ in range(V)]

    def add_vertex(self, node_id : int = 0, data : dict = {}) -> None:
        self.index_map[node_id] = self.last_index
        self.nodes[self.last_index] = copy.deepcopy(data)
        self.nodes[self.last_index]["node id"] = node_id
        self.last_index += 1

    def add_edge(self, node_id1 : int = 0, node_id2 : int = 0) -> None:
        if node_id1 == node_id2:
            raise ValueError("A node cannot be its own neighbor.")
        index1 = verify_key(self.index_map, node_id1)
        index2 = verify_key(self.index_map, node_id2)

        if index1 in self.adjList[index2] or index2 in self.adjList[index1]:
            raise ValueError("Cannot add same node twice or more.")

        self.adjList[index1].append(index2)
        self.adjList[index2].append(index1)

    def get_node_data(self, node_id : int = 0) -> dict:
        """
        Returns a reference to a given node's data, allowing the user to
        modify it.
        """
        index = verify_key(self.index_map, node_id)
        return copy.deepcopy(self.nodes[index])
# Several nodes
# List of neighbors
