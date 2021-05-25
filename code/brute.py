from scc import Graph, Node, visualize
from t4 import read_graph


def reachable(root: Node) -> set[Node]:
    result = set()

    def visit(node: Node) -> None:
        result.add(node)

        for v in node.adj():
            if not v in result:
                visit(v)
    visit(root)
    return result


def unreachable(root: Node) -> set[Node]:
    reach = reachable(root)
    return {v for v in root.graph.vertices() if v not in reach}


if __name__ == "__main__":
    G = read_graph()

    # for node in G.vertices():
    #     R = unreachable(node)
    #     if len(R) > 0:
    #         print(node, R)

    for node in G.vertices():
        R = unreachable(node)
        if len(R) > 0:
            print("Inadequado.")
            break
    else:
        print("Adequado.")
