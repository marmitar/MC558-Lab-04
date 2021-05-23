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

// Representação de um nó por lista de adjacências.
typedef struct node {
    value_t value;  // valor do nó
    size_t len;     // qtde de nós adjacentes
    size_t cap;     // capacidade da lista
    value_t adj[];  // índice dos nós adjacentes
} node_t;

// Representação do grafo por lista das adjacências de cada nó.
typedef struct graph {
    size_t size;    // qtde de nós no grafo
    node_t *node[]; // listas das adjacências de cada nó
} graph_t;

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
    if unlikely(rv < 0) {
        graph_free(graph);
        return EXIT_FAILURE;
    }
    // exibição da adequação
    if (rv > 0) {
        printf("Adequado.\n");
    } else {
        printf("Inadequado.\n");
    }

    graph_free(graph);
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
node_t *node_new(value_t value) {
    const size_t INITIAL = 16;

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
        node_t *node = node_new(u);
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
    // valores devem ser índices válidos
    if unlikely(from >= graph->size || to >= graph->size) {
        return false;
    }
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
    for (size_t i = 0; i < max; i++) {
        size_t A, B; uint8_t D;
        // leitura dos cruzamento e da direção
        int rv = scanf("%zu %zu %hhu", &A, &B, &D);
        // EOF, continua válido
        if unlikely(rv < 3) return true;

        switch (D) {
            case 2:
                // conexão da mão contrária
                bool ok = graph_connect(graph, B, A);
                if unlikely(!ok) return false;
            case 1:
                // e da mão direta
                ok = graph_connect(graph, A, B);
                if unlikely(!ok) return false;
            break;
            default:
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

// Erro durante teste.
#define ERROR (-1)

/* Requisitos de trânsito. */
int pass_transit_requirements(const graph_t *_map) {
    return ERROR;
}
