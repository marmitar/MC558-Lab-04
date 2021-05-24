from typing import Iterator
from scc import Graph, SCC, DFS, visualize


def numbers() -> Iterator[list[int]]:
    try:
        while True:
            yield [int(s) for s in input().split()]
    except EOFError:
        pass

if __name__ == "__main__":
    G = Graph()

    N, _ = next(numbers())
    for i in range(N):
        G.add(str(i))

    for A, B, D in numbers():
        A = G[str(A)]
        B = G[str(B)]

        if D == 1:
            A.add_edge(B)
        elif D == 2:
            A.add_edge(B)
            B.add_edge(A)
        else:
            raise ValueError(D)

    if len(SCC(G).roots) <= 1:
        print("Adequado.")
    else:
        print("Inadequado.")
