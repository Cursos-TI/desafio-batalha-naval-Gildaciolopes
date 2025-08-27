#include <stdio.h>
#include <stdbool.h>

#define ROWS 10
#define COLS 10
#define SHIP_SIZE 3
#define WATER 0
#define SHIP 3

/* Verifica se dá para posicionar horizontalmente (linha fixa, colunas incrementam) */
bool can_place_horizontal(int board[ROWS][COLS], int sr, int sc) {
    if (sr < 0 || sr >= ROWS) return false;
    if (sc < 0 || sc + SHIP_SIZE > COLS) return false;
    for (int c = sc; c < sc + SHIP_SIZE; ++c)
        if (board[sr][c] != WATER) return false;
    return true;
}

/* Verifica se dá para posicionar verticalmente (coluna fixa, linhas incrementam) */
bool can_place_vertical(int board[ROWS][COLS], int sr, int sc) {
    if (sc < 0 || sc >= COLS) return false;
    if (sr < 0 || sr + SHIP_SIZE > ROWS) return false;
    for (int r = sr; r < sr + SHIP_SIZE; ++r)
        if (board[r][sc] != WATER) return false;
    return true;
}

/* Verifica se dá para posicionar diagonal DOWN-RIGHT (linha++ e coluna++) */
bool can_place_diag_dr(int board[ROWS][COLS], int sr, int sc) {
    if (sr < 0 || sr + SHIP_SIZE > ROWS) return false;
    if (sc < 0 || sc + SHIP_SIZE > COLS) return false;
    for (int i = 0; i < SHIP_SIZE; ++i)
        if (board[sr + i][sc + i] != WATER) return false;
    return true;
}

/* Verifica se dá para posicionar diagonal DOWN-LEFT (linha++ e coluna--) */
bool can_place_diag_dl(int board[ROWS][COLS], int sr, int sc) {
    if (sr < 0 || sr + SHIP_SIZE > ROWS) return false;
    if (sc < 0 || sc - (SHIP_SIZE - 1) < 0) return false;
    for (int i = 0; i < SHIP_SIZE; ++i)
        if (board[sr + i][sc - i] != WATER) return false;
    return true;
}

/* Coloca navio horizontal (sr,sc) ... (sr,sc+SHIP_SIZE-1) */
void place_horizontal(int board[ROWS][COLS], int sr, int sc) {
    for (int c = sc; c < sc + SHIP_SIZE; ++c) board[sr][c] = SHIP;
}

/* Coloca navio vertical (sr,sc) ... (sr+SHIP_SIZE-1,sc) */
void place_vertical(int board[ROWS][COLS], int sr, int sc) {
    for (int r = sr; r < sr + SHIP_SIZE; ++r) board[r][sc] = SHIP;
}

/* Coloca navio diagonal DOWN-RIGHT */
void place_diag_dr(int board[ROWS][COLS], int sr, int sc) {
    for (int i = 0; i < SHIP_SIZE; ++i) board[sr + i][sc + i] = SHIP;
}

/* Coloca navio diagonal DOWN-LEFT */
void place_diag_dl(int board[ROWS][COLS], int sr, int sc) {
    for (int i = 0; i < SHIP_SIZE; ++i) board[sr + i][sc - i] = SHIP;
}

/* Imprime o tabuleiro com índices */
void print_board(int board[ROWS][COLS]) {
    printf("   ");
    for (int c = 0; c < COLS; ++c) printf("%d ", c);
    printf("\n");
    for (int r = 0; r < ROWS; ++r) {
        printf("%2d ", r);
        for (int c = 0; c < COLS; ++c) {
            printf("%d ", board[r][c]);
        }
        printf("\n");
    }
}

int main(void) {
    int board[ROWS][COLS];

    /* Inicializa o tabuleiro com água (0) */
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            board[r][c] = WATER;

    /* ----------------- Coordenadas pré-definidas dos 4 navios -----------------
       Observação: índices 0-based (0..9)
       - Navio 1 (horizontal): começa em (1,1) -> ocupa (1,1),(1,2),(1,3)
       - Navio 2 (vertical)  : começa em (4,6) -> ocupa (4,6),(5,6),(6,6)
       - Navio 3 (diag ↓→)   : começa em (7,0) -> ocupa (7,0),(8,1),(9,2)
       - Navio 4 (diag ↓←)   : começa em (0,9) -> ocupa (0,9),(1,8),(2,7)
       Estas posições foram escolhidas para ficar dentro do tabuleiro e sem sobreposição.
    -------------------------------------------------------------------------*/
    int h_sr = 1, h_sc = 1;   /* horizontal */
    int v_sr = 4, v_sc = 6;   /* vertical */
    int dr_sr = 7, dr_sc = 0; /* diagonal down-right */
    int dl_sr = 0, dl_sc = 9; /* diagonal down-left */

    /* Valida e posiciona navio horizontal */
    if (!can_place_horizontal(board, h_sr, h_sc)) {
        printf("Erro: não é possível posicionar o navio horizontal em (%d,%d).\n", h_sr, h_sc);
        return 1;
    }
    place_horizontal(board, h_sr, h_sc);

    /* Valida e posiciona navio vertical */
    if (!can_place_vertical(board, v_sr, v_sc)) {
        printf("Erro: não é possível posicionar o navio vertical em (%d,%d).\n", v_sr, v_sc);
        return 1;
    }
    place_vertical(board, v_sr, v_sc);

    /* Valida e posiciona navio diagonal DOWN-RIGHT */
    if (!can_place_diag_dr(board, dr_sr, dr_sc)) {
        printf("Erro: não é possível posicionar o navio diagonal (down-right) em (%d,%d).\n", dr_sr, dr_sc);
        return 1;
    }
    place_diag_dr(board, dr_sr, dr_sc);

    /* Valida e posiciona navio diagonal DOWN-LEFT */
    if (!can_place_diag_dl(board, dl_sr, dl_sc)) {
        printf("Erro: não é possível posicionar o navio diagonal (down-left) em (%d,%d).\n", dl_sr, dl_sc);
        return 1;
    }
    place_diag_dl(board, dl_sr, dl_sc);

    /* Exibe tabuleiro final */
    printf("Tabuleiro com 4 navios posicionados (0 = água, 3 = navio):\n");
    print_board(board);

    return 0;
}
