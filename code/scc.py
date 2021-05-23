from __future__ import annotations
from collections import defaultdict
from typing import Callable, Iterable, Iterator, Optional, Union
from enum import Enum, auto, unique
from dataclasses import dataclass


class Graph:
    def __init__(self):
        self.nodes: dict[str, Node] = {}

    def __getitem__(self, name: str) -> Node:
        return self.nodes[name]

    def __contains__(self, name: str) -> bool:
        return name in self.nodes

    def get(self, name: str) -> Optional[Node]:
        return self.get(name, None)

    def get_or_create(self, name: str) -> Node:
        if name not in self:
            self.nodes[name] = Node(self, name)
        return self[name]

    def add(self, name: str, *edges: str) -> Node:
        node = self.get_or_create(name)
        node.add_edge(*edges)
        return node

    def vertices(self) -> Iterator[Node]:
        for _, node in self.nodes.items():
            yield node

    def __str__(self) -> str:
        nodes = ','.join(self.nodes)
        return self.__class__.__name__ + '({' + nodes + '})'


class Node:
    def __init__(self, graph: Graph, name: str):
        self.graph = graph
        self.name = name
        self.adjacency: set[Node] = set()

    def add_edge(self, *nodes: Union[str, Node]) -> None:
        for node in nodes:
            name = node if isinstance(node, str) else node.name

            node = self.graph.get_or_create(name)
            self.adjacency.add(node)

    def adj(self) -> Iterator[Node]:
        return iter(self.adjacency)

    def __hash__(self) -> int:
        return hash(self.name)

    def __str__(self) -> str:
        return self.name

    def __repr__(self) -> str:
        return f'{self.__class__.__name__}({self.name})'


@unique
class Color(Enum):
    White = auto()
    Gray  = auto()
    Black = auto()


@dataclass
class Data:
    color: defaultdict[Node, Color]
    parent: dict[Node, Node]
    time: int
    discovery: dict[Node, int]
    finalization: dict[Node, int]
    roots: list[Node]

    def __init__(self) -> None:
        self.color = defaultdict(lambda: Color.White)
        self.parent = {}
        self.time = 0
        self.discovery = {}
        self.finalization = {}
        self.roots = []

    def discover(self, node: Node) -> int:
        time = self.time
        self.discovery[node] = time
        self.time = time + 1

        self.color[node] = Color.Gray
        return time

    def finalize(self, node: Node) -> int:
        time = self.time
        self.finalization[node] = time
        self.time = time + 1

        self.color[node] = Color.Black
        return time

    def treesets(self) -> Iterator[set[Node]]:
        for root in self.roots:
            yield treeset(root, self.parent)


def DFS(graph: Graph, *, sort: Callable[[Iterable[Node]], Iterator[Node]]=lambda x: x) -> Data:
    data = Data()

    def visit(u: Node):
        data.discover(u)
        for v in sort(u.adj()):
            if data.color[v] == Color.White:
                data.parent[v] = u
                visit(v)
        data.finalize(u)

    for node in sort(graph.vertices()):
        if data.color[node] == Color.White:
            visit(node)
            data.roots.append(node)

    return data


def SCC(graph: Graph) -> Data:
    transposed = Graph()
    for node in graph.vertices():
        transposed.add(node.name)
        for adj in node.adj():
            adj = transposed.get_or_create(adj.name)
            adj.add_edge(node.name)

    finished = DFS(transposed).finalization
    def finalization(node: Node) -> int:
        return finished[transposed[node.name]]
    def sort(nodes: Iterable[Node]) -> list[Node]:
        return sorted(nodes, key=finalization, reverse=True)

    return DFS(graph, sort=sort)


def treeset(root: Node, parent: dict[Node, Node]) -> set[Node]:
        nodes = {root}
        for v in root.adj():
            if parent.get(v) is root:
                nodes |= treeset(v, parent)
        return nodes


G = Graph()
G.add('a', 'b')
G.add('b', 'e', 'f', 'c')
G.add('c', 'g', 'd')
G.add('d', 'c', 'h')
G.add('e', 'a', 'f')
G.add('f', 'g')
G.add('g', 'f', 'h')
G.add('h', 'h')

for node_set in DFS(G).treesets():
    print(node_set)

print()
for node_set in SCC(G).treesets():
    print(node_set)
