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
