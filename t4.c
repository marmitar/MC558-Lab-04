/**
 * MC558 - Teste 04
 * Tiago de Paula - RA 187679
 */
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
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


typedef struct matrix {
    size_t len;
    bool v[];
} mat_t;


static attribute(malloc, hot, nothrow)
mat_t *mat_alloc(size_t len) {
    mat_t *M = calloc(offsetof(mat_t, v) + len * len * sizeof(bool), 1);
    if unlikely(M == NULL) return NULL;

    M->len = len;
    return M;
}

#define at(M, i, j) \
    M->v[(i) * M->len + (j)]

static attribute(malloc, cold, nothrow)
mat_t *read_graph(void) {
    size_t N, M;
    int rv = scanf("%zu %zu", &N, &M);
    if unlikely(rv < 2 || N * sizeof(bool) >= SIZE_MAX / N) return NULL;

    mat_t *G = mat_alloc(N);
    if unlikely(G == NULL) return NULL;

    for (size_t i = 0; i < M; i++) {
        size_t A, B;
        uint8_t D;
        rv = scanf("%zu %zu %hhu", &A, &B, &D);
        if unlikely(rv < 3 || A >= N || B >= N) {
            return G;
            // free(G);
            // return NULL;
        }

        switch (D) {
            case 2:
                at(G, B, A) = true;
            case 1:
                at(G, A, B) = true;
                break;
            default:
                return G;
                // free(G);
                // return NULL;
        }
    }
    return G;
}

static attribute(access(write_only, 1), access(read_only, 2), nonnull, hot, nothrow)
void mat_copy(mat_t *restrict dest, const mat_t *restrict src) {
    // assert(dest->len == src->len)
    memcpy(dest->v, src->v, src->len * src->len * sizeof(bool));
}

static attribute(access(write_only, 1), access(read_only, 2), access(read_only, 3), nonnull, hot, nothrow)
void mat_mul(mat_t *restrict out, const mat_t *A, const mat_t *B) {
    mat_copy(out, A);

    for (size_t i = 0; i < A->len; i++) {
        for (size_t j = 0; j < A->len; j++) {
            for (size_t k = 0; k < A->len; k++) {
                at(out, i, j) |= at(A, i, k) & at(B, k, j);
            }
        }
    }
}

static attribute(access(write_only, 1), access(read_only, 2), nonnull, hot, nothrow)
void mat_square(mat_t *restrict out, const mat_t *restrict in) {
    mat_mul(out, in, in);
}

static attribute(access(write_only, 1), cold, nothrow)
void mat_eye(mat_t *M) {
    for (size_t i = 0; i < M->len; i++) {
        at(M, i, i) = true;
    }
}

static attribute(malloc, access(read_only, 1), nonnull, cold, nothrow)
mat_t *mat_alloc_copy(const mat_t *M) {
    mat_t *C = mat_alloc(M->len);
    if unlikely(C == NULL) return NULL;
    mat_copy(C, M);
    return C;
}

static attribute(access(read_write, 1), malloc, nonnull, hot, nothrow)
mat_t *mat_pow(mat_t *M, size_t n) {
    mat_t *S = (n % 2 == 0)? mat_alloc(M->len) : mat_alloc_copy(M);
    if unlikely(S == NULL) return NULL;
    mat_eye(S);

    if unlikely(n < 2) return S;

    mat_t *A = M;
    mat_t *B = mat_alloc(M->len);
    if unlikely(B == NULL) {
        free(S);
        free(B);
        return NULL;
    }

    for (n /= 2; n > 0; n /= 2) {
        {
            mat_square(B, A);
            mat_t *ptr = A;
            A = B;
            B = ptr;
        }

        if (n % 2 == 1) {
            mat_mul(B, S, A);
            mat_t *ptr = S;
            S = B;
            B = ptr;
        }
    }
    free(A); free(B);
    return S;
}

static attribute(access(read_only, 1), nonnull, cold, nothrow)
bool mat_all(const mat_t *mat) {
    for (size_t i = 0; i < mat->len; i++) {
        for (size_t j = 0; j < mat->len; j++) {
            if unlikely(! at(mat, i, j)) {
                return false;
            }
        }
    }
    return true;
}

int main(void) {
    mat_t *graph = read_graph();
    if unlikely(graph == NULL) {
        return EXIT_FAILURE;
    }

    mat_t *path = mat_pow(graph, graph->len * graph->len);
    if unlikely(path == NULL) {
        free(graph);
        return EXIT_FAILURE;
    }

    if (mat_all(path)) {
        printf("Adequado.\n");
    } else {
        printf("Inadequado.\n");
    }
    free(path);
    return EXIT_SUCCESS;
}
