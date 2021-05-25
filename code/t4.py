from dataclasses import dataclass
from enum import unique
from typing import Iterator, Optional, TypeVar, Union
from scc import Graph, Node, visualize


def numbers(max: Optional[int]=None) -> Iterator[list[int]]:
    try:
        if isinstance(max, int):
            for _ in range(max):
                yield [int(s) for s in input().split()]
        else:
            while True:
                yield [int(s) for s in input().split()]
    except EOFError:
        pass


def read_graph() -> Graph:
    G = Graph()
    N, M = next(numbers())

    for i in range(N):
        G.add(str(i))

    for A, B, D in numbers(M):
        A = G[str(A)]
        B = G[str(B)]

        if D == 1:
            A.add_edge(B)
        elif D == 2:
            A.add_edge(B)
            B.add_edge(A)
        else:
            raise ValueError(D)

    return G

@dataclass
class Data:
    vertices: int
    preorder: dict[Node, int]
    unassigned: list[Node]
    undetermined: list[Node]
    components: list[set[Node]]

    def __init__(self) -> None:
        self.vertices = 0
        self.preorder = {}
        self.unassigned = []
        self.undetermined = []
        self.components = []

    def visited(self, node: Node) -> bool:
        return node in self.preorder

    def discover(self, node: Node) -> int:
        vertices = self.vertices
        self.preorder[node] = vertices
        self.vertices = vertices + 1

        self.unassigned.append(node)
        self.undetermined.append(node)
        return vertices

    @property
    def head(self) -> Node:
        return self.undetermined[-1]

    def pop_for(self, node: Union[int, Node]) -> int:
        if not isinstance(node, int):
            node = self.preorder[node]

        count = 0
        while self.preorder[self.head] > node:
            self.undetermined.pop()
            count += 1
        return count

    def new_component(self, node: Node) -> set[Node]:
        component = {self.undetermined.pop()}
        while (v := self.unassigned.pop()) is not node:
            component.add(v)

        self.components.append(component)
        return component



def cSCC(graph: Graph) -> list[set[Node]]:
    data = Data()

    def forget(*args):
        pass

    def show(*args):
        print(*args)
        print()

    def visit(node: Node, print=forget):
        data.discover(node)
        print(node, 'disc', data.preorder[node], data.head)

        for neighbor in node.adj():
            print(node, 'neigh', neighbor, data.unassigned, data.undetermined)

            if not data.visited(neighbor):
                visit(neighbor, print)
            elif neighbor in data.unassigned:
                data.pop_for(neighbor)

        print(node, 'head', data.head, data.unassigned, data.undetermined)
        if node is data.head:
            data.new_component(node)

    for node in graph.vertices():
        if not data.visited(node):
            # if node.name not in "
            visit(node)

    # print(data.preorder[graph['6']])
    # print(data)
    return data.components


T = TypeVar('T')

def union(*sets: set[T]) -> set[T]:
    result = set()
    for s in sets:
        result = result.union(s)
    return result

def intersection(*sets: set[T]) -> set[T]:
    try:
        result = sets[0]
    except IndexError:
        return set()

    for s in sets[1:]:
        result.intersection_update(s)
    return result


if __name__ == "__main__":
    G = read_graph()

    C = cSCC(G)
    # print(C, len(C[0]), sum(1 for _ in G.vertices()))
    # print(set(G.vertices()).difference(*C))

    if len(C) == 1:
        print("Adequado.")
    else:
        print("Inadequado.")

    # visualize(G)
