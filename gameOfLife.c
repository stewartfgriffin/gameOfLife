#include <stdlib.h>
#include <stdbool.h>
#include <error.h>
#include <assert.h>
#include <stdio.h>

#define UNLIMITED_TURNCOUNT 0

#define LIVE true
#define DEAD false
typedef bool health;

#define SUCCESS true
#define FAILED false
typedef bool errorFlag;

typedef struct {
    health **cells;
    int width, height;
} grid;

grid *createGrid(int width, int height){
    grid *g = malloc(sizeof(grid));
    if (!g) return g;
    g->width = width;
    g->height = height;
    g->cells = malloc(width * sizeof(health *));
    for (int i = 0; i < width; i++){
            g->cells[i] = malloc(height * sizeof(health));
        for (int j = 0; j < height; j++) {
            g->cells[i][j] = DEAD;
        }
    }
    return g;
}

void releaseGrid(grid *g) {
    if (!g) return;
    for (int i = 0; i < g->width; i++) {
        free(g->cells[i]);
    }
    free(g->cells);
    free(g); 
}

grid *copyGrid(grid *g) {
    grid *copy = malloc(sizeof(grid));
    if (!copy) return copy;
    assert(g->width > 0 && g->height > 0);
    copy->width = g->width;
    copy->height = g->height;
    assert(g->cells);
    copy->cells = malloc(g->width * sizeof(health *));
    for (int i = 0; i < g->width; i++) {
        copy->cells[i] = malloc(g->height * sizeof(health));
        for (int j = 0; j < g->height; j++) {
            copy->cells[i][j] = g->cells[i][j];
        }
    }

    return copy;
}

errorFlag createLife(grid *g, int x, int y) {
    if (x < 0 || y < 0 || x > g->width-1 || y > g->height-1) return FAILED;    
    g->cells[x][y] = LIVE;
}

int neighbourCount(grid *g, int x, int y) {
    assert(x >= 0 && y >= 0 && x < g->width && y < g->height);
    int neighbours = 0;
    int minX = x > 0 ? x - 1 : x;
    int minY = y > 0 ? y - 1 : y;
    int maxX = x < g->width-1 ? x + 1 : x;
    int maxY = y < g->height-1 ? y + 1 : y;

    for (int i = minX; i <= maxX; i++) {
        for (int j = minY; j <= maxY; j++) {
            if (i == x && j == y) continue;
            if (g->cells[i][j] == LIVE) neighbours++;
        }
    }
    return neighbours;
}

health vitality(grid *g, int x, int y) {
    int neighbours = neighbourCount(g, x, y);
    if (g->cells[x][y] == LIVE) {
        return neighbours == 2 || neighbours == 3 ? LIVE : DEAD;
    } else {
        return neighbours == 3 ? LIVE : DEAD;
    }
}


void displayGrid(grid *g) {
    for (int i = 0; i < g->width; i++){
        for (int j = 0; j < g->height; j++) {
            printf("|");
            printf(g->cells[i][j] == LIVE ? "x" : "-");
        }
        printf("|\n");
    }
}

errorFlag runLife(grid *g, int maxTurns) {
    grid *current = copyGrid(g); //use copy of g for mem man purposes
    grid *next;
    for (int turn = 0; maxTurns == 0 || turn < maxTurns; turn++) {
        printf("Life as we know it in turn %d:\n", turn);
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

void failed() {
    exit(EXIT_FAILURE);
}

void succeeded() {
    exit(EXIT_SUCCESS);
}

void main(int* argc, int** argv) {
    grid *g = createGrid(20, 20); 
    if (!g) failed();
    createLife(g, 9, 9);
    createLife(g, 8, 9);
    createLife(g, 7, 9);

    errorFlag e = runLife(g, 5);
    releaseGrid(g);
    if (e == FAILED) failed();
    succeeded();
}
