#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "poke327-s2023-1.01/poke327.c"


#define WORLD_SIZE 401

//in Y, X
map_t *world[WORLD_SIZE][WORLD_SIZE];
int curPos[2] = {200, 200};


int main(int argc, char *argv[]) {
    int i, j, nx, ny;
    char in;

    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            world[i][j] = NULL;
        }
    }

    new_map(world[curPos[0]][curPos[1]]);

    print_map(world[curPos[0]][curPos[1]]);

    while (1) {
        switch (in) {
            case 'n':
                curPos[0]++;
                break;
            case 's':
                curPos[0]--;
                break;
            case 'e':
                curPos[1]++;
                break;
            case 'w':
                curPos[1]--;
                break;
            case 'f':
                curPos[0] = ny;
                curPos[1] = nx;
                break;
            case 'q':
                return 0;
                break;
            default:
                printf("Command Not Valid");
                break;
        }

        if (!(curPos[0] % 401)) {
            curPos[0]--;
        }
        if (!(curPos[1] % 401)) {
            curPos[1]--;
        }
        if (world[curPos[0]][curPos[1]] == NULL) {
            new_map(world[curPos[0]][curPos[1]]);
        }
        print_map(world[curPos[0]][curPos[1]]);

        printf("Current Position: (%d, %d). Enter Move:\t", curPos[1] - 200, curPos[0] - 200);
        in = scanf("%c %d %d", in, nx, ny);
    }

    return 1;
}