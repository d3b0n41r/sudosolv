#include <stdio.h>
#include <string.h>
#include <curses.h> // pacman -S ncurses
#include <locale.h> // allows overscore UTF-8 using printw

// define
#define SIZE 9

// global
int board[SIZE][SIZE];
int row, col;

// make board
void init_board();
void get_board_values();

// display board
void display_board();
void set_terminal_yellow();
void set_terminal_reset();
void print_yellow_bar();
void print_row_split(int y);

// 'cursor' but not really
void get_arrows_pressed(int c);
void set_cursor_red_background();
void reset_cursor_red_background(); 

// solve board
int is_valid(int ro, int co, int val);
int solve_board_recur(int r, int c);


int main(void) {
    // start curses.h
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_color(COLOR_WHITE, 1000, 1000, 1000);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_MAGENTA);
    curs_set(0);

    // make board
    init_board();
    display_board();
    get_board_values();

    clear();
    display_board();
    getch();

    // end curses.h
    endwin();
    curs_set(1);

    return 0;
}
// initialise board with values
void init_board() {
    for(int y = 0; y < SIZE; y++) {
        for(int x = 0; x < SIZE; x++) {
            board[y][x] = 0;
        }
    }
}
// update board pointers for arrow keys pressed (start [0][0])
void get_arrows_pressed(int c) {
    switch(c) {
        case 259: // key up
            if(row == 0)
                row = SIZE - 1;
            else
                row -= 1;
            break;
        case 258: // key down
            if(row == SIZE - 1)
                row = 0;
            else
                row += 1;
            break;
        case 261: // key right
            if(col == SIZE - 1)
                col = 0;
            else 
                col += 1;
            break;
        case 260: // key left
            if(col == 0)
                col = SIZE - 1;
            else
                col -= 1;
            break;
    }

}
// get values to solve
void get_board_values() {
    int c;
    do {
        c = getch();
        clear();
        get_arrows_pressed(c);
        display_board();
        if(c >= '0' && c <= '9') { // number key pressed
            board[row][col] = c - 48;
            clear();
            display_board();
        }
    }while (c != 'q');
    // solve
    solve_board_recur(0, 0);
}
int is_valid(int ro, int co, int val) {
    int i, j, r, c;

    if(board[ro][co] != 0) {
        if(board[ro][co] != val)
            return 0;
    }
    for(i = 0; i < 9; i++) {
        if(i != co)
            if(board[ro][i] == val)
                return 0;
        if(i != ro)
            if(board[i][co] == val)
                return 0;
    }
    r = (ro / 3) * 3;
    c = (co / 3) * 3;
    for(i = r; i < r + 3; i++)
        for(j = c; j < c + 3; j++)
            if(i != ro || j != co)
                if(board[i][j] == val)
                    return 0;
    
    return 1;
}
// solve board
int solve_board_recur(int r, int c) {
    int num = 1;

    if(c == 9) {
        c = 0;
        r++;
    }
    if(r == 9) {
        return 1;
    }
    while (num < 10) {
        if(is_valid(r, c, num)) { 
            board[r][c] = num;
            if(solve_board_recur(r, c+1))
                return 1;
        }
        board[r][c] = 0;
        num++;
    }
    return 0;
}
// print board
void display_board() {
    printw("\n press q to solve\n");
    for(int y = 0; y < SIZE; y++) {
        printw("\n");
        print_row_split(y);
        print_yellow_bar();

        for(int x = 0; x < SIZE; x++) {
            // highlight pointer location relative to board
            if(row == y && col == x) {
                set_cursor_red_background();
                printw("%d", board[y][x]);
                reset_cursor_red_background();
            }
            else
                printw("%d", board[y][x]);
            if((x+1)%3==0 && x != SIZE - 1) {
                print_yellow_bar();
                printw(" ");
                print_yellow_bar();
            }
            else if(x == SIZE - 1)
                print_yellow_bar();
            else {
                set_terminal_reset();
                printw("|");
            }
        }
    }
    printw("\n");
    print_row_split(9); // use overlines instead of underlines
    printw("\n");
}
// gaps between 3x3 grids makes board more easily read
void print_row_split(int y) {
    char score[5];
    if(y%3==0 && y!=9) {
        printw(" ");
        strcpy(score, "_ ");
    }
    else if(y==9) {
        strcpy(score, "\u203E "); 
        printw(" ");
    }
    else
        return;
    for(int i = 0; i < 9; i++) {
        if((i)%3 == 0 && i != 0)
            printw("  ");
        set_terminal_yellow();
        printw("%s", score);
        set_terminal_reset();
    }
    printw("\n");
}
// yellow bar for 3x3 grids that make board more easily read
void print_yellow_bar() {
    set_terminal_yellow();
    printw("|");
    set_terminal_reset();
}

// highlight cursor position with red background for cursor position on board
void set_cursor_red_background() {
    attron(COLOR_PAIR(2));
}
void reset_cursor_red_background() {
    attroff(COLOR_PAIR(2));
}

// change printw colours for board clarity
void set_terminal_yellow() {
    attron(COLOR_PAIR(1) | A_BOLD);
    // printf("\033[1;32m");
}
void set_terminal_reset() {
    attroff(COLOR_PAIR(1) | A_BOLD);
    // printf("\033[0m");
}