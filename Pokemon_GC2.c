#include "lib/maps.c"
#include "lib/trainers.c"


//globals
Map *world[WORLD_SIZE][WORLD_SIZE];
Map *trails[num_types_tra];
Point curPos, center;


//initers
int init_world()
{
    int i, j;

    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            world[i][j] = NULL;
        }
    }

    return 0;
}

int init_trails()
{
    int i;


}

//display updates
int add_entity_trainer(Entity *e, char map[BOUNDS_Y][BOUNDS_X][10])
{
    strcpy(map[e->pos.y][e->pos.x], COLOR_RESET);
    strcat(map[e->pos.y][e->pos.x], (char[2]) {ALL_TRAINERS[e->chr], '\0'});

    return 0;
}

int add_trainer(Trainer *t, char map[BOUNDS_Y][BOUNDS_X][10])
{
    add_entity_trainer(&t->e, map);

    return 0;
}

int update_trails(PC *player, Trainer** t)
{
    int i;
    int numTrainers;

    numTrainers = sizeof(**t)/sizeof(Trainer);
    // printf("numTrainers: %d\n", numTrainers);

    dijkstra(trails[PLAY], world[curPos.y][curPos.x], player->e.pos, &player->e, NULL);
    for (i = 0; i < numTrainers; i++) {
        dijkstra(trails[t[i]->e.chr], world[curPos.y][curPos.x], t[i]->e.pos, &t[i]->e, startTer[i]);
    }

    return 0;
}

int map_chars(Map *from, char map[BOUNDS_Y][BOUNDS_X][10])
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            strcpy(map[i][j], TERRAIN_COLORS[from->terrain[i][j]]);
            strcat(map[i][j], (char[2]) {ALL_TERRAIN[from->terrain[i][j]], '\0'});
            // printf("x:%d, y:%d, chr:%c\n", j, i, ALL_TERRAIN[from->terrain[i][j]]);
        }
    }

    return 0;
}

//The actual game
int gameloop(int numTrainers)
{
    if (!init_world()) {
        
    }
}


int main(int argc, char *argv[])
{
    int numTrainers = 10;

    if (argc > 1 && argc != 3) {
        printf("Usage: %s --numtrainers <number of trainers to spawn <default: 10>\n", argv[0]);
        return 1;
    } else if (strcmp(argv[1], "--numtrainers") == 0) {
        numTrainers = atoi(argv[2]);
    }

    return gameloop(numTrainers);
}