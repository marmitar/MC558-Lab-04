/**
 * MC558 - Teste 04
 * Tiago de Paula - RA 187679
 */
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>


#ifdef __GNUC__
// Atributos do GCC
#define attribute(...) __attribute__((__VA_ARGS__))
// Marcador de branch improvável (usado para erros)
#define unlikely(x)    (__builtin_expect((x), 0))

#else
// Fora do GCC, as macros não fazem nada.
#define attribute(...)
#define unlikely(x)      (x)
#endif

// Índice dos vértices.
typedef uint16_t value_t;

typedef struct graph graph_t;
// Representação de um nó por lista de adjacências.
typedef struct node {
    value_t value;  // valor do nó
    size_t len;     // qtde de nós adjacentes
    size_t cap;     // capacidade da lista
    value_t adj[];  // índice dos nós adjacentes
} node_t;

// Representação do grafo por lista das adjacências de cada nó.
struct graph {
    size_t size;    // qtde de nós no grafo
    node_t *node[]; // listas das adjacências de cada nó
};

static
/**
 * Leitura do grafo pela entrada padrão.
 *
 * Retorna NULL em caso de falha.
 */
graph_t *read_graph(void)
attribute(malloc, cold, nothrow);


static attribute(cold, nothrow)
/**
 * Desaloca o grafo e seus nós.
 */
void graph_free(graph_t *graph) {
    for (size_t u = 0; u < graph->size; u++) {
        free(graph->node[u]);
    }
    free(graph);
}

static
/**
 * Checagem dos requisitos do mapa.
 *
 * Retorna 0 se não passar nos requisitos, um valor
 * positivo se passar ou -1 em caso de erro.
 */
int pass_transit_requirements(const graph_t *map)
attribute(pure, nonnull, hot, nothrow);

/* Entrada do programa. */
int main(void) {
    // leitura
    graph_t *graph = read_graph();
    if unlikely(graph == NULL) {
        return EXIT_FAILURE;
    }

    // teste da adequação
    int rv = pass_transit_requirements(graph);
    graph_free(graph); // não é mais necessário
    if unlikely(rv < 0) return EXIT_FAILURE;

    // exibição da adequação
    if (rv > 0) {
        printf("Adequado.\n");
    } else {
        printf("Inadequado.\n");
    }
    return EXIT_SUCCESS;
}

/* * * * * * * * * * *
 * ALOCAÇÃO DO GRAFO *
 * * * * * * * * * * */

static attribute(malloc, cold, nothrow)
/**
 * Aloca novo nó com capcacidade inicial de 16.
 *
 * Retorna NULL em caso de falha.
 */
node_t *node_new(value_t value, graph_t *graph) {
    const size_t INITIAL = 16;

    node_t *new = malloc(offsetof(node_t, adj) + INITIAL * sizeof(value_t));
    if unlikely(new == NULL) return NULL;

    new->cap = INITIAL;
    new->len = 0; // marca a lista como vazia
    new->value = value;
    return new;
}

static attribute(malloc, cold, nothrow)
/**
 * Duplica a capacidade da lista de adj. do nó.
 *
 * Retorna NULL em caso de falha, sem desalocar
 * o nó anterior.
 */
node_t *node_increase(node_t *node) {
    size_t capacity = node->cap * 2;
    // realoca com dobro de capacidade
    node_t *new = realloc(node, offsetof(node_t, adj) + capacity * sizeof(value_t));
    if unlikely(new == NULL) return NULL;

    // ajusta a capacidade
    new->cap = capacity;
    return new;
}

static attribute(malloc, cold, nothrow)
/**
 * Aloca grafo e as listas de adjacências de cada nó.
 *
 * Retorna NULL em caso de falha.
 */
graph_t *graph_new(size_t size) {
    graph_t *new = malloc(offsetof(graph_t, node) + size * sizeof(node_t *));
    if unlikely(new == NULL) return NULL;

    new->size = 0;
    for (size_t u = 0; u < size; u++) {
        // aloca nó
        node_t *node = node_new(u, new);
        if unlikely(node == NULL) {
            graph_free(new);
            return NULL;
        }
        // insere no grafo
        new->node[u] = node;
        new->size += 1;
    }
    return new;
}

static attribute(nonnull, cold, nothrow)
/**
 * Cria aresta direcionada de 'from' para 'to'.
 *
 * Retorna 'true' em caso de sucesso, 'false' caso contrário.
 */
bool graph_connect(graph_t *graph, value_t from, value_t to) {
    node_t *node = graph->node[from];
    // aumenta o tamanho se necessário
    if unlikely(node->len >= node->cap) {
        node_t *new = node_increase(node);
        if unlikely(new == NULL) return false;
        // reinsere no grafo
        node = graph->node[from] = new;
    }

    // insera a aresta na lista de adjacências
    node->adj[node->len++] = to;
    return true;
}

static attribute(nonnull, cold, nothrow)
/**
 * Leitura das arestas do grafo.
 *
 * Retorna 'true' em caso de sucesso, 'false' caso contrário.
 */
bool read_edges(graph_t *graph, size_t max) {
    size_t size = graph->size;

    for (size_t i = 0; i < max; i++) {
        size_t A, B; uint8_t D;
        // leitura dos cruzamento e da direção
        int rv = scanf("%zu %zu %hhu", &A, &B, &D);
        // EOF, continua válido
        if unlikely(rv < 0) return true;
        if unlikely(rv < 3 || A >= size || B >= size) return false;

        // conexão de duas mãos
        if (D == 2) {
            bool ok = graph_connect(graph, B, A);
            if unlikely(!ok) return false;
            ok = graph_connect(graph, A, B);
            if unlikely(!ok) return false;
        // conexão da mão direta
        } else if (D == 1) {
            bool ok = graph_connect(graph, A, B);
            if unlikely(!ok) return false;
        // entrada inválida
        } else {
            return false;
        }
    }
    return true;
}

/* Leitura do grafo. */
graph_t *read_graph(void) {
    size_t N, M;
    // dimensões do grafo
    int rv = scanf("%zu %zu", &N, &M);
    if unlikely(rv < 2 || N >= UINT16_MAX) return NULL;

    // alocação
    graph_t *G = graph_new(N);
    if unlikely(G == NULL) return NULL;

    // leitura das arestas
    bool ok = read_edges(G, M);
    if unlikely(!ok) {
        graph_free(G);
        return NULL;
    }
    return G;
}


/* * * * * * * * * * *
 * CHECAGEM DO MAPA  *
 * * * * * * * * * * */

// Pilha de vértices com CFG a determinar.
typedef struct stack {
    size_t len;
    // marcador de momento de inserção
    size_t clock;
    value_t data[];
} stack_t;

static attribute(malloc, cold, nothrow)
/**
 * Cria nova pilha com capacidade limitada.
 * Retorna NULL em erro de alocação.
 */
stack_t *stack_new(size_t capacity) {
    const size_t size = offsetof(stack_t, data) + capacity * sizeof(value_t);
    stack_t *new = malloc(size);
    if unlikely(new == NULL) return NULL;

    new->len = 0; // vazia
    new->clock = 0;
    return new;
}

static inline attribute(pure, hot, nonnull, nothrow)
/**
 * Próximo elemento a ser removido.
 */
value_t stack_head(const stack_t *stack) {
    // assume pilha não vazia
    size_t last = stack->len - 1;
    return stack->data[last];
}

static attribute(hot, nonnull, nothrow)
/**
 * Insere na pilha e retorna o momento da inserção.
 */
value_t stack_push(stack_t *stack, value_t node) {
    value_t time = ++stack->clock;
    // assume que não ultrapassa a capacidade
    stack->data[stack->len++] = node;
    return time;
}

static inline attribute(hot, nonnull, nothrow)
/**
 * Remove úlyimo elemento da pilha.
 */
void stack_pop(stack_t *stack) {
    // assume pilha não vazia
    stack->len--;
}

// posição pré-ordem de nó não visitado
#define UNVISITED 0
// maior número de CFG válidos
#define MAX_SCC 1

// Estrutura com informação usada na contagem de GFCs.
struct info {
    // pilha de nós sem componente
    stack_t * restrict unassigned;
    // ordem de inserção na pilha (e busca pré-ordem)
    value_t * restrict preorder;
};

static attribute(nonnull, cold, nothrow)
/**
 * Inicializa a estrutura para a contagem.
 */
bool info_init(struct info *info, size_t size) {
    // a pilha vai ter nó máximo todos os vértices do grafo
    stack_t *unassigned = stack_new(size);
    if unlikely(unassigned == NULL) return false;
    // o momento zero indica não visitado
    value_t *preorder = calloc(size, sizeof(value_t));
    if unlikely(preorder == NULL) {
        free(unassigned);
        return false;
    };

    *info = (struct info) {
        .unassigned = unassigned,
        .preorder = preorder
    };
    return true;
}

static inline attribute(nonnull, hot, nothrow)
/**
 * Marca nó como descoberto na estrutura da informação.
 */
void info_discover(const struct info info, value_t node) {
    info.preorder[node] = stack_push(info.unassigned, node);
}

static inline attribute(pure, nonnull, hot, nothrow)
/**
 * Teste se nó ainda não foi visitado.
 */
bool info_unvisited(const struct info info, value_t node) {
    return info.preorder[node] == UNVISITED;
}

static inline attribute(pure, nonnull, hot, nothrow)
/**
 * Checa se o nó está na pilha de não determinados.
 */
bool info_unassigned(const struct info info, value_t node) {
    // value_t order = info.preorder[node];
    // // limites da busca
    // size_t lo = 0, hi = info.unassigned->len;

    // while (lo < hi) {
    //     size_t mid = (lo + hi) / 2;
    //     // preoreder da posição 'mid' da pilha
    //     value_t pmid = info.preorder[info.unassigned->data[mid]];

    //     if (pmid < order) {
    //         hi = mid;
    //     } else if (pmid > order) {
    //         lo = mid + 1;
    //     } else {
    //         return true;
    //     }
    // }
    // return false;

    for (size_t i = info.unassigned->len; i > 0; i--) {
        if (info.unassigned->data[i-1] == node) {
            return true;
        }
    }
    return false;
}

static inline attribute(hot, nothrow)
/**
 * Marca nós como parte do CFG atual.
 */
void info_assign_until_node(const struct info info, value_t node) {
    value_t order = info.preorder[node];

    // remove da pilha de não determinados
    while (info.preorder[stack_head(info.unassigned)] > order) {
        stack_pop(info.unassigned);
    }
}

static inline attribute(hot, nothrow)
/**
 * Teste se é possível criar um CFG com o nó atual.
 */
bool info_new_component(const struct info info, value_t node) {
    if (stack_head(info.unassigned) == node) {
        // remove o nó, pois será parte do CFG
        stack_pop(info.unassigned);
        return true;
    } else {
        return false;
    }
}

static attribute(nonnull, hot, nothrow)
/**
 * Visita o nós como no DFS, mas retorna a quantidade de Componentes
 * Fortemente Conexos encontrada.
 *
 * Retorna antes se MAX_SCC foi alcançado.
 */
size_t SCC_visit(const node_t * restrict node, const graph_t * restrict graph, const struct info info) {
    info_discover(info, node->value);

    // quantidade de CFGs
    size_t count = 0;
    for (size_t i = node->len; i > 0; i--) {
        value_t adj = node->adj[i-1];

        // visita não visitado
        if (info_unvisited(info, adj)) {
            count += SCC_visit(graph->node[adj], graph, info);
            if unlikely(count > MAX_SCC) return count;
        // ou marca como parte do CFG
        } else if (info_unassigned(info, adj)) {
            info_assign_until_node(info, adj);
        }
    }

    // tenta finalizar o CFG
    if (info_new_component(info, node->value)) {
        count += 1;
    }
    return count;
}

static attribute(nonnull, hot, nothrow)
/**
 * Retorna a quantidade de CFGs no grafo.
 */
size_t SCC_count(const graph_t * restrict graph, const struct info info) {
    size_t count = 0;

    for (size_t i = graph->size; i > 0; i--) {
        if (info_unvisited(info, i-1)) {
            count += SCC_visit(graph->node[i-1], graph, info);
            // retorna já estorou o limite
            if unlikely(count > MAX_SCC) return count;
        }
    }
    return count;
}

// Erro durante teste.
#define ERROR (-1)

/* Requisitos de trânsito. */
int pass_transit_requirements(const graph_t *map) {
    // zero pode dar problema na alocação
    if unlikely(map->size == 0) return false;

    struct info info;
    if unlikely(!info_init(&info, map->size)) return ERROR;

    // conta os CFGs no mapa
    size_t sccs = SCC_count(map, info);
    free(info.preorder);
    free(info.unassigned);
    // que deve ser apenas 1
    return sccs == MAX_SCC;
}
