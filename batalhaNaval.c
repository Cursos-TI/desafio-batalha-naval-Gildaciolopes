/* batalha_mestre.c
   Nível Mestre — Batalha Naval com Habilidades (Cone, Cruz, Octaedro)
   0 = água
   3 = navio
   5 = área de habilidade
   8 = sobreposição (navio + área de habilidade)
*/

#include <stdio.h>
#include <stdbool.h>

#define ROWS 10
#define COLS 10
#define SHIP_SIZE 3
#define WATER 0
#define SHIP 3
#define EFFECT 5
#define OVERLAP 8

/* Funções de validação de posicionamento (horizontal/vertical/diagonais) */
bool can_place_horizontal(int board[ROWS][COLS], int sr, int sc) {
    if (sr < 0 || sr >= ROWS) return false;
    if (sc < 0 || sc + SHIP_SIZE > COLS) return false;
    for (int c = sc; c < sc + SHIP_SIZE; ++c)
        if (board[sr][c] != WATER) return false;
    return true;
}

bool can_place_vertical(int board[ROWS][COLS], int sr, int sc) {
    if (sc < 0 || sc >= COLS) return false;
    if (sr < 0 || sr + SHIP_SIZE > ROWS) return false;
    for (int r = sr; r < sr + SHIP_SIZE; ++r)
        if (board[r][sc] != WATER) return false;
    return true;
}

bool can_place_diag_dr(int board[ROWS][COLS], int sr, int sc) {
    if (sr < 0 || sr + SHIP_SIZE > ROWS) return false;
    if (sc < 0 || sc + SHIP_SIZE > COLS) return false;
    for (int i = 0; i < SHIP_SIZE; ++i)
        if (board[sr + i][sc + i] != WATER) return false;
    return true;
}

bool can_place_diag_dl(int board[ROWS][COLS], int sr, int sc) {
    if (sr < 0 || sr + SHIP_SIZE > ROWS) return false;
    if (sc < 0 || sc - (SHIP_SIZE - 1) < 0) return false;
    for (int i = 0; i < SHIP_SIZE; ++i)
        if (board[sr + i][sc - i] != WATER) return false;
    return true;
}

/* Funções para colocar navios no tabuleiro */
void place_horizontal(int board[ROWS][COLS], int sr, int sc) {
    for (int c = sc; c < sc + SHIP_SIZE; ++c) board[sr][c] = SHIP;
}
void place_vertical(int board[ROWS][COLS], int sr, int sc) {
    for (int r = sr; r < sr + SHIP_SIZE; ++r) board[r][sc] = SHIP;
}
void place_diag_dr(int board[ROWS][COLS], int sr, int sc) {
    for (int i = 0; i < SHIP_SIZE; ++i) board[sr + i][sc + i] = SHIP;
}
void place_diag_dl(int board[ROWS][COLS], int sr, int sc) {
    for (int i = 0; i < SHIP_SIZE; ++i) board[sr + i][sc - i] = SHIP;
}

/* Imprime o tabuleiro final com legenda.
   Prioridade de exibição:
   - OVERLAP (8) se navio e área de habilidade sobrepõem
   - EFFECT (5) se área mas sem navio
   - SHIP (3) se navio mas sem área
   - WATER (0) caso contrário
*/
void print_board(int combined[ROWS][COLS]) {
    printf("Legenda: 0=água  3=navio  5=efeito  8=navio+efeito (sobreposição)\n\n");
    printf("   ");
    for (int c = 0; c < COLS; ++c) printf("%d ", c);
    printf("\n");
    for (int r = 0; r < ROWS; ++r) {
        printf("%2d ", r);
        for (int c = 0; c < COLS; ++c) {
            printf("%d ", combined[r][c]);
        }
        printf("\n");
    }
}

/* Constrói matrizes de habilidade (tamanho odd, aqui usamos 5x5).
   Cada matriz é criada usando loops aninhados e condicionais.
   - cone: triângulo invertido apontando para baixo (ponto no topo).
   - cross: linha e coluna centrais marcadas.
   - octa (losango): pontos onde distância de Manhattan <= radius.
*/
void build_cone(int M, int cone[M][M]) {
    int center = M / 2; // índice central
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < M; ++c) {
            // cone apontando para baixo: em linha r (0=topo) marque colunas
            // no intervalo [center - r, center + r], limitado ao M.
            if ( (center - r) <= c && c <= (center + r) ) cone[r][c] = 1;
            else cone[r][c] = 0;
        }
    }
}

void build_cross(int M, int cross[M][M]) {
    int center = M / 2;
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < M; ++c) {
            // Cruz: marque a linha central e a coluna central
            if (r == center || c == center) cross[r][c] = 1;
            else cross[r][c] = 0;
        }
    }
}

void build_octa(int M, int octa[M][M]) {
    int center = M / 2;
    int radius = center; // por exemplo, para M=5 radius=2
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < M; ++c) {
            // octaedro (vista frontal) ~ losango: Manhattan distance <= radius
            int dist = (r > center ? r - center : center - r) + (c > center ? c - center : center - c);
            if (dist <= radius) octa[r][c] = 1;
            else octa[r][c] = 0;
        }
    }
}

/* Sobrepõe uma matriz de habilidade (mat[M][M]) no tabuleiro 'combined'.
   origin_r/origin_c é o ponto do tabuleiro que será o centro da habilidade.
   center é o índice central da matriz (M/2).
   Se uma posição for afetada, marca Efeito (EFFECT) a menos que já tenha navio -> então marca OVERLAP.
   Usamos uma matriz 'base' com navios para saber se havia navio.
*/
void apply_effect(int base[ROWS][COLS], int combined[ROWS][COLS],
                  int matSize, int mat[matSize][matSize],
                  int origin_r, int origin_c) {
    int center = matSize / 2;
    for (int mr = 0; mr < matSize; ++mr) {
        for (int mc = 0; mc < matSize; ++mc) {
            if (mat[mr][mc] != 1) continue; // não faz nada se não é parte da área
            int br = origin_r - center + mr; // mapeia para tabuleiro
            int bc = origin_c - center + mc;
            if (br < 0 || br >= ROWS || bc < 0 || bc >= COLS) continue; // fora dos limites
            // se já contém navio na base (3), marca overlap (8)
            if (base[br][bc] == SHIP) combined[br][bc] = OVERLAP;
            else combined[br][bc] = EFFECT;
        }
    }
}

int main(void) {
    int board[ROWS][COLS];       // tabuleiro base com navios (0 ou 3)
    int combined[ROWS][COLS];    // tabuleiro final com efeitos aplicados

    /* Inicializa tabuleiro com água */
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c) {
            board[r][c] = WATER;
            combined[r][c] = WATER;
        }

    /* ----------------- Posicionamento de 4 navios (pré-definidos) -----------------
       (mesma lógica do nível anterior)
       - Navio 1 (horizontal): (1,1) -> (1,1..3)
       - Navio 2 (vertical)  : (4,6) -> (4..6,6)
       - Navio 3 (diag ↓→)   : (7,0) -> (7,0),(8,1),(9,2)
       - Navio 4 (diag ↓←)   : (0,9) -> (0,9),(1,8),(2,7)
    -----------------------------------------------------------------------------*/
    int h_sr = 1, h_sc = 1;   /* horizontal */
    int v_sr = 4, v_sc = 6;   /* vertical */
    int dr_sr = 7, dr_sc = 0; /* diagonal down-right */
    int dl_sr = 0, dl_sc = 9; /* diagonal down-left */

    /* Valida e posiciona navios (se algum falhar, encerra com erro) */
    if (!can_place_horizontal(board, h_sr, h_sc)) {
        printf("Erro: não é possível posicionar o navio horizontal em (%d,%d).\n", h_sr, h_sc);
        return 1;
    }
    place_horizontal(board, h_sr, h_sc);

    if (!can_place_vertical(board, v_sr, v_sc)) {
        printf("Erro: não é possível posicionar o navio vertical em (%d,%d).\n", v_sr, v_sc);
        return 1;
    }
    place_vertical(board, v_sr, v_sc);

    if (!can_place_diag_dr(board, dr_sr, dr_sc)) {
        printf("Erro: não é possível posicionar o navio diagonal DR em (%d,%d).\n", dr_sr, dr_sc);
        return 1;
    }
    place_diag_dr(board, dr_sr, dr_sc);

    if (!can_place_diag_dl(board, dl_sr, dl_sc)) {
        printf("Erro: não é possível posicionar o navio diagonal DL em (%d,%d).\n", dl_sr, dl_sc);
        return 1;
    }
    place_diag_dl(board, dl_sr, dl_sc);

    /* Copia base para combined inicialmente (navios copiados) */
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            combined[r][c] = board[r][c];

    /* ----------------- Construção das matrizes de habilidade (5x5) -----------------*/
    const int M = 5;
    int cone[M][M];
    int cross[M][M];
    int octa[M][M];

    build_cone(M, cone);   /* cone apontando para baixo (topo no índice 0) */
    build_cross(M, cross);
    build_octa(M, octa);

    /* ----------------- Definição dos pontos de origem (no tabuleiro) -------------
       Estes pontos são definidos diretamente no código (exemplos).
       Ajuste as coordenadas para testar outros posicionamentos.
    ---------------------------------------------------------------------------*/
    int cone_origin_r = 0, cone_origin_c = 2;   /* cone com topo perto do topo do tabuleiro */
    int cross_origin_r = 5, cross_origin_c = 5; /* cruz centrada no meio-direito */
    int octa_origin_r = 3, octa_origin_c = 2;   /* octaedro (losango) perto do centro-esquerdo */

    /* ----------------- Aplicação das habilidades ao tabuleiro ------------------*/
    apply_effect(board, combined, M, cone, cone_origin_r, cone_origin_c);
    apply_effect(board, combined, M, cross, cross_origin_r, cross_origin_c);
    apply_effect(board, combined, M, octa, octa_origin_r, octa_origin_c);

    /* ----------------- Exibição final do tabuleiro -----------------------------*/
    printf("Tabuleiro final com navios (3) e áreas de habilidade (5). Sobreposição = 8.\n\n");
    print_board(combined);

    /* Opcional: para visualização rápida, também imprimimos as matrizes de habilidade */
    printf("\nMatriz CONE (1=afetado, 0=não afetado):\n");
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < M; ++c) printf("%d ", cone[r][c]);
        printf("\n");
    }

    printf("\nMatriz CROSS (1=afetado, 0=não afetado):\n");
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < M; ++c) printf("%d ", cross[r][c]);
        printf("\n");
    }

    printf("\nMatriz OCTA (1=afetado, 0=não afetado):\n");
    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < M; ++c) printf("%d ", octa[r][c]);
        printf("\n");
    }

    return 0;
}
