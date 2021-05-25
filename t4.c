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
    const size_t INITIAL = 8;

    node_t *new = malloc(offsetof(node_t, adj) + INITIAL * sizeof(uint16_t));
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
    node_t *new = realloc(node, offsetof(node_t, adj) + capacity * sizeof(uint16_t));
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
        if unlikely(rv < 3) return true;
        if unlikely(A >= size || B >= size) return false;

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

// Nó e posição dele no percurso pré-ordem da floresta BP.
typedef struct data {
    value_t node;
    value_t preorder;
} data_t;

// Pilha de vértices com CFG a determinar.
typedef struct stack {
    size_t len;
    // momento de inserção (usado como posição pré-ordem)
    value_t clock;
    data_t data[];
} stack_t;

static attribute(malloc, cold, nothrow)
/**
 * Cria nova pilha com capacidade limitada.
 * Retorna NULL em erro de alocação.
 */
stack_t *stack_new(size_t capacity) {
    const size_t size = offsetof(stack_t, data) + capacity * sizeof(data_t);
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
data_t stack_head(const stack_t *stack) {
    // assume pilha não vazia
    size_t last = stack->len - 1;
    return stack->data[last];
}

static attribute(hot, nonnull, nothrow)
/**
 * Insere na pilha.
 */
value_t stack_push(stack_t *stack, value_t node) {
    value_t preorder = ++stack->clock;
    // assume que não ultrapassa a capacidade
    stack->data[stack->len++] = (data_t) {
        .node = node,
        .preorder = preorder
    };
    return preorder;
}

static inline attribute(hot, nonnull, nothrow)
/**
 * Remove úlyimo elemento da pilha.
 */
data_t stack_pop(stack_t *stack) {
    // assume pilha não vazia
    return stack->data[--stack->len];
}

static attribute(pure, hot, nonnull, nothrow)
/**
 * Checa se a pilha com o nó com pré-ordem 'preorder'
 * por busca binária. (Os elementos são inseridos nessa ordem).
 */
bool stack_contains(const stack_t *stack, value_t preorder) {
    // limites da busca
    size_t lo = 0, hi = stack->len;

    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        value_t pmid = stack->data[mid].preorder;

        if (pmid < preorder) {
            hi = mid;
        } else if (pmid > preorder) {
            lo = mid + 1;
        } else {
            return true;
        }
    }
    return false;
}

// Erro durante teste.
#define ERROR (-1)

/* Requisitos de trânsito. */
int pass_transit_requirements(const graph_t *_map) {
    return ERROR;
}
