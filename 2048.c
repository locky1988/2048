#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <time.h>

int grid_arr[4][4];
int score = 0;
int best_score = 0;
WINDOW *w_frame = NULL;

int gen_empty_arr(int empty_arr[]) ;
int rand_a_num(int from, int to) ;
int init_game() ;
int add_num() ;
int free_game() ;
int disp_grid() ;
int disp_one_grid(int x, int y, int num, WINDOW **w_one) ;
int left_move() ;
int merge_line(int arr[]) ;
int trans_right() ;
int trans_up() ;
int retrans_up() ;
int can_move() ;
int compute_color(int num) ;

int gen_empty_arr(int empty_arr[]) {
    int i = 0, j = 0, k = 0;

    for(i=0; i<4; i++) {
        for(j=0; j<4; j++) {
            if(grid_arr[i][j]==0) empty_arr[k++] = i*10+j;
        }
    }

    return k;
}

int rand_a_num(int from, int to) {
    int tmp=0;
    if(from>to) tmp=from,from=to,to=tmp;

    return (rand()%(to-from+1))+from;
}

int init_game() {
    FILE *fp = NULL;

    fp = fopen(".2048.dat", "r");
    if (fp != (FILE*)NULL) {
        fscanf(fp, "%d", &best_score);
        fclose(fp);
    }

    srand(time(NULL));
    initscr();
    curs_set(0);
    noecho();
    refresh();

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
    init_pair(5, COLOR_BLACK, COLOR_BLUE);
    init_pair(6, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(7, COLOR_BLACK, COLOR_CYAN);
    init_pair(8, COLOR_WHITE, COLOR_RED);
    init_pair(9, COLOR_WHITE, COLOR_GREEN);
    init_pair(10, COLOR_WHITE, COLOR_YELLOW);
    init_pair(11, COLOR_WHITE, COLOR_BLUE);
    init_pair(12, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(13, COLOR_WHITE, COLOR_CYAN);

    w_frame = newwin(26, 32, LINES/2-13, COLS/2-16);
    keypad(w_frame, TRUE);
    box(w_frame, 0, 0);
    mvwaddstr(w_frame, 1, 14, "2048");
    wrefresh(w_frame);

    add_num();
    add_num();
    return 0;
}

int add_num() {
    int empty_arr[16];
    int size = 0;
    int randi = 0;
    int randn = 0;
    int ret = 0;

    size = gen_empty_arr(empty_arr);

    randi = rand_a_num(0, size-1);
    randn = rand_a_num(0, 1);

    grid_arr[empty_arr[randi]/10][empty_arr[randi]%10] = randn==0?2:4;
    disp_grid();
    
    if(size==1 && can_move()==0) ret = -1; 

    return ret;
}

int free_game() {
    delwin(w_frame);
    endwin();
    return 0;
}

int disp_grid() {
    int i = 0;
    int j = 0;
    WINDOW *w_tmp = NULL;
    char str[20] = {0};

    for(i=0; i<4; i++) {
        for(j=0; j<4; j++) {
            disp_one_grid(3+j*5, 2+i*7, grid_arr[j][i], &w_tmp); 
        }
    }

    delwin(w_tmp);

    sprintf(str, "Score:%-6d   Best:%-6d", score, best_score);
    mvwaddstr(w_frame, 24, 4, str);
    wrefresh(w_frame);
    return 0;
}

int disp_one_grid(int x, int y, int num, WINDOW **w_one) {
    WINDOW *w_tmp = NULL;
    char numstr[5] = {0};
    int color = 0;
    int flag = 0;

    w_tmp = derwin(w_frame, 5, 7, x, y);
    werase(w_tmp);

    if(num) {
        color = compute_color(num);

        wattron(w_tmp, COLOR_PAIR(color));
        mvwaddstr(w_tmp, 1, 1, "     ");
        mvwaddstr(w_tmp, 2, 1, "     ");
        sprintf(numstr, "%d", num);
        mvwaddstr(w_tmp, 2, num>999?1:(num>9?2:3), numstr);
        mvwaddstr(w_tmp, 3, 1, "     ");
        wattroff(w_tmp, COLOR_PAIR(color));
    }

    box(w_tmp, 0, 0);
    wrefresh(w_tmp);

    *w_one = w_tmp;
    return 0;
}

int left_move() {
    int i = 0;
    int flag = 0;

    for(i=0; i<4; i++) {
        if(1==merge_line(grid_arr[i])) flag = 1;
    }

    return flag;
}

int merge_line(int arr[]) {
    int i = 0;
    int j = 0;
    int n = 0;
    int flag = 0;

    for(i=0; i<3; i++) {
        for(n=0; arr[i]==0&&n<3; n++) {
            for(j=i; j<3; j++) {
                arr[j] = arr[j+1];
                if(arr[j+1]!=0) flag = 1;
                arr[j+1] = 0;
            }
        }
    }

    for(i=1; i<4; i++) {
        if(arr[i]==arr[i-1] && arr[i]) {
            score += arr[i]*2; 
            arr[i-1] = arr[i]*2;
            arr[i] = 0;
            for(j=i+1; j<4; j++) {
                arr[j-1] = arr[j]; 
                arr[j] = 0;
            }
            flag = 1;
        }
    }

    return flag; 
}

int trans_right() {
    int i = 0;
    int tmp = 0;

    for(i=0; i<4; i++) {
        tmp = grid_arr[i][0];
        grid_arr[i][0] = grid_arr[i][3]; 
        grid_arr[i][3] = tmp;

        tmp = grid_arr[i][1];
        grid_arr[i][1] = grid_arr[i][2]; 
        grid_arr[i][2] = tmp;
    }

    return 0;
}

int trans_up() {
    int i = 0;
    int j = 0;
    int tmparr[4][4];

    for(i=0; i<4; i++)
        for(j=0; j<4; j++)
            tmparr[i][j] = grid_arr[j][3-i];   

    for(i=0; i<4; i++)
        for(j=0; j<4; j++)
            grid_arr[i][j] = tmparr[i][j];   

    return 0;
}

int retrans_up() {
    int i = 0;
    int j = 0;
    int tmparr[4][4];

    for(i=0; i<4; i++)
        for(j=0; j<4; j++)
            tmparr[i][j] = grid_arr[3-j][i];   

    for(i=0; i<4; i++)
        for(j=0; j<4; j++)
            grid_arr[i][j] = tmparr[i][j];   

    return 0;
}

int can_move() {
    int i = 0;
    int flag = 0;

    for(i=0; i<4; i++) {
        if(grid_arr[i][1]==grid_arr[i][0] || grid_arr[i][2]==grid_arr[i][1] || grid_arr[i][3]==grid_arr[i][2]) {
            flag = 1;
            break;
        }
    }

    if(flag==0) {
        for(i=0; i<4; i++) {
            if(grid_arr[1][i]==grid_arr[0][i] || grid_arr[2][i]==grid_arr[1][i] || grid_arr[3][i]==grid_arr[2][i]) {
                flag = 1;
                break;
            }
        }
    }

    return flag;
}

int compute_color(int num) {
    int i = 0;

    for(i=0; num!=1; i++) num = num>>1; 

    return i;
}

int main() {
    int key = 0;
    int flag = 0;
    int ret = 0;
    FILE *fp = NULL;

    init_game();

    while(1) {
        disp_grid();
        
        do{
            key = wgetch(w_frame);
        } while(key!=KEY_UP && key!=KEY_DOWN && key!=KEY_LEFT && key!=KEY_RIGHT);

        switch(key) {
            case KEY_LEFT:
                flag = left_move(); 
                break; 
            case KEY_RIGHT:
                trans_right();
                flag = left_move();
                trans_right();
                break;
            case KEY_UP:
                trans_up();
                flag = left_move();
                retrans_up();
                break;
            case KEY_DOWN:
                retrans_up();
                flag = left_move();
                trans_up();
                break;
        }

        if(flag) {
            ret = add_num();
            if(ret==-1) {
                mvwaddstr(w_frame, 2, 7, "You lose! Press Enter to exit!");

                if(best_score<score) {
                    fp = fopen(".2048.dat", "w");
                    if (fp != (FILE*)NULL) {
                        fprintf(fp, "%d", score);
                        fclose(fp);
                    }
                }

                break;
            }
        }
    }

    do{ key = wgetch(w_frame); }while(key!=10);

    free_game();

    return 0;
}
