from typing import Iterator, Optional
from scc import Graph, SCC, DFS, visualize


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


if __name__ == "__main__":
    G = read_graph()

    if len(SCC(G).roots) == 1:
        print("Adequado.")
    else:
        print("Inadequado.")

    # visualize(G)
