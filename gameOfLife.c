#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

#include "point.h"
#include "error.h"
#include "grid.h"
#include "config.h"
#include "parseArgs.h"

#define UNLIMITED_TURNCOUNT 0
#define DEFAULT_MAX_TURNS 1
#define DEFAULT_GRID_WIDTH 20
#define DEFAULT_GRID_HEIGHT 20

errorFlag createLife(grid *gameGrid, int x, int y) {
    if (x < 0 || y < 0 || x > gameGrid->width-1 || y > gameGrid->height-1) {
        return FAILED;
    }     
    gameGrid->cells[x][y] = LIVE;
    return SUCCESS;
}

int neighbourCount(grid *gameGrid, int x, int y) {
    assert(x >= 0 && y >= 0 && x < gameGrid->width && y < gameGrid->height);
    int neighbours = 0;
    int minX = x > 0 ? x - 1 : x;
    int minY = y > 0 ? y - 1 : y;
    int maxX = x < gameGrid->width-1 ? x + 1 : x;
    int maxY = y < gameGrid->height-1 ? y + 1 : y;

    for (int i = minX; i <= maxX; i++) {
        for (int j = minY; j <= maxY; j++) {
            if (i == x && j == y) continue;
            if (gameGrid->cells[i][j] == LIVE) neighbours++;
        }
    }
    return neighbours;
}

health vitality(grid *gameGrid, int x, int y) {
    int neighbours = neighbourCount(gameGrid, x, y);
    if (gameGrid->cells[x][y] == LIVE) {
        return neighbours == 2 || neighbours == 3 ? LIVE : DEAD;
    } else {
        return neighbours == 3 ? LIVE : DEAD;
    }
}

void displayGrid(grid *gameGrid) {
    for (int i = 0; i < gameGrid->width; i++){
        for (int j = 0; j < gameGrid->height; j++) {
            printf("|");
            printf(gameGrid->cells[i][j] == LIVE ? "x" : "-");
        }
        printf("|\n");
    }
}

errorFlag runLife(grid *gameGrid, int maxTurns) {
    grid *current = copyGrid(gameGrid); //use copy of g for mem man purposes
    if (!current) return FAILED;
    grid *next;
    for (int turn = 0; maxTurns == 0 || turn < maxTurns; turn++) {
        printf("Life as we know it in turn %d:\n", turn+1);
        displayGrid(current);
        printf("\n");
        next = copyGrid(current);
        if (!next) return FAILED;
        for (int i = 0; i < current->width; i++){
            for (int j = 0; j < current->height; j++) {
                next->cells[i][j] = vitality(current, i, j);
            }
        }
        releaseGrid(current);
        current = next;
    }
    releaseGrid(current);
    return SUCCESS;
}

errorFlag start(config *cfg) {
    grid *gameGrid = createGrid(cfg); 
    if (!gameGrid) return FAILED;
    
    pointNode *livePointNode = cfg->firstPointNode;
    while (livePointNode && livePointNode->value) {
        createLife(gameGrid, livePointNode->value->x, livePointNode->value->y);
        livePointNode = livePointNode->next;
    }

    errorFlag e = runLife(gameGrid, cfg->maxTurns);
    releaseGrid(gameGrid);

    return e;
}

void failed() {
    exit(EXIT_FAILURE);
}

void succeeded() {
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
    config *cfg = NULL;
    errorFlag e = SUCCESS;
    cfg = createConfig(
        DEFAULT_GRID_WIDTH, DEFAULT_GRID_HEIGHT, DEFAULT_MAX_TURNS);
    if (!cfg) {
        e = FAILED;
    } else if (argc > 1) {
        e = parseArgs(cfg, argc, argv);
    }
    if (e == SUCCESS) e = start(cfg);

    releaseConfig(cfg);

    if (e == FAILED) failed();
    succeeded();
}
