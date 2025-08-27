#include <stdio.h>
#include <stdbool.h>

#define ROWS 10
#define COLS 10
#define SHIP_SIZE 3
#define WATER 0
#define SHIP 3

/* Verifica se dá para posicionar horizontalmente (sem sair dos limites e sem sobreposição) */
bool can_place_horizontal(int board[ROWS][COLS], int start_row, int start_col) {
    if (start_row < 0 || start_row >= ROWS) return false;
    if (start_col < 0 || start_col + SHIP_SIZE > COLS) return false; // +SHI P_SIZE porque ocupa posições [start_col .. start_col+SHIP_SIZE-1]
    for (int c = start_col; c < start_col + SHIP_SIZE; ++c) {
        if (board[start_row][c] != WATER) return false; // já ocupado
    }
    return true;
}

/* Verifica se dá para posicionar verticalmente (sem sair dos limites e sem sobreposição) */
bool can_place_vertical(int board[ROWS][COLS], int start_row, int start_col) {
    if (start_col < 0 || start_col >= COLS) return false;
    if (start_row < 0 || start_row + SHIP_SIZE > ROWS) return false;
    for (int r = start_row; r < start_row + SHIP_SIZE; ++r) {
        if (board[r][start_col] != WATER) return false; // já ocupado
    }
    return true;
}

/* Copia o navio (valor SHIP) para o tabuleiro horizontalmente */
void place_horizontal(int board[ROWS][COLS], int start_row, int start_col) {
    for (int c = start_col; c < start_col + SHIP_SIZE; ++c) {
        board[start_row][c] = SHIP;
    }
}

/* Copia o navio (valor SHIP) para o tabuleiro verticalmente */
void place_vertical(int board[ROWS][COLS], int start_row, int start_col) {
    for (int r = start_row; r < start_row + SHIP_SIZE; ++r) {
        board[r][start_col] = SHIP;
    }
}

/* Imprime o tabuleiro com cabeçalho de colunas para facilitar visualização */
void print_board(int board[ROWS][COLS]) {
    printf("   "); // espaço para índice de linha
    for (int c = 0; c < COLS; ++c) {
        printf("%d ", c);
    }
    printf("\n");

    for (int r = 0; r < ROWS; ++r) {
        printf("%2d ", r); // índice da linha
        for (int c = 0; c < COLS; ++c) {
            printf("%d ", board[r][c]);
        }
        printf("\n");
    }
}

int main(void) {
    /* Inicializa o tabuleiro com água (0) */
    int board[ROWS][COLS];
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            board[r][c] = WATER;

    /* --------- Coordenadas dos navios (definidas no código) ---------
       OBS: os índices são 0-based (linha 0..9, coluna 0..9)
       Altere estes valores para testar outros posicionamentos.
       Exemplo atual:
         - navio_horizontal começa em linha 2, coluna 4 (ocupa 2,4 - 2,6)
         - navio_vertical começa em linha 5, coluna 7 (ocupa 5,7 - 7,7)
    -----------------------------------------------------------------*/
    int horiz_start_row = 2;
    int horiz_start_col = 4;

    int vert_start_row = 5;
    int vert_start_col = 7;

    /* Valida e posiciona navio horizontal */
    if (!can_place_horizontal(board, horiz_start_row, horiz_start_col)) {
        printf("Erro: não é possível posicionar o navio horizontal em (%d,%d).\n",
               horiz_start_row, horiz_start_col);
        return 1;
    }
    place_horizontal(board, horiz_start_row, horiz_start_col);

    /* Valida e posiciona navio vertical (garante que não sobrescreva o horizontal) */
    if (!can_place_vertical(board, vert_start_row, vert_start_col)) {
        printf("Erro: não é possível posicionar o navio vertical em (%d,%d).\n",
               vert_start_row, vert_start_col);
        return 1;
    }
    place_vertical(board, vert_start_row, vert_start_col);

    /* Exibe o tabuleiro final */
    printf("Tabuleiro com navios posicionados (0 = água, 3 = navio):\n");
    print_board(board);

    return 0;
}
