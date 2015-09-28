#include <stdlib.h>
#include <stdbool.h>
#include <error.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define UNLIMITED_TURNCOUNT 0
#define DEFAULT_MAX_TURNS 1
#define DEFAULT_GRID_WIDTH 20
#define DEFAULT_GRID_HEIGHT 20

#define LIVE true
#define DEAD false
typedef bool health;

#define SUCCESS true
#define FAILED false
typedef bool errorFlag;

typedef struct point {
    int x, y;
} point;

point *createPoint(int x, int y) {
    point *p = malloc(sizeof(point));
    if (!p) return NULL;
    p->x = x;
    p->y = y;
    return p;
}

void releasePoint(point *p){
    if(!p) return;
    free(p);
}

typedef struct pointNode {
    point *value;
    struct pointNode *next;
} pointNode;

pointNode *createPointNode(point *p, pointNode *next) {
    pointNode *pn = malloc(sizeof(pointNode));
    if (!pn) return pn;
    pn->value = p;
    pn->next = next;
    return pn;
}

void releasePointNode(pointNode *pn) {
    if (!pn) return;
    pointNode *current = pn;
    int i = 0;
    while (current) {
        pointNode *next = current->next;
        releasePoint(current->value);
        free(current);
        current = next;
    }
}

pointNode *copyPointNode(pointNode *pn) {
    pointNode *copy = malloc(sizeof(pointNode));
    copy->value = pn->value;
    copy->next = pn->next;
    return copy;
}

typedef struct config {
    int width, height, maxTurns;
    pointNode *firstPointNode;
} config;

config *createConfig(int width, int height, int maxTurns) {
   config *cfg = malloc(sizeof(config));
   if (!cfg) return cfg;
   cfg->width = width;
   cfg->height = height;
   cfg->maxTurns = maxTurns;
   cfg->firstPointNode = NULL;

   return cfg;
}

void releaseConfig(config *cfg) {
    if (!cfg)  return;
    releasePointNode(cfg->firstPointNode);
    free(cfg);
}

typedef struct grid {
    health **cells;
    int width, height;
} grid;

grid *createGrid(config *cfg) {
    grid *g = malloc(sizeof(grid));
    if (!g) return g;
    g->width = cfg->width;
    g->height = cfg->height;
    g->cells = malloc(cfg->width * sizeof(health *));
    if (!g->cells) return NULL;
    for (int i = 0; i < cfg->width; i++){
        g->cells[i] = malloc(cfg->height * sizeof(health));
        if (!g->cells[i]) return NULL;
        for (int j = 0; j < cfg->height; j++) {
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
    if (!copy->cells) return NULL;
    for (int i = 0; i < g->width; i++) {
        copy->cells[i] = malloc(g->height * sizeof(health));
        if (!copy->cells[i]) return NULL;
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
    if (!current) return FAILED;
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

errorFlag start(config *cfg) {
    grid *g = createGrid(cfg); 
    if (!g) failed();
    
    pointNode *livePointNode = cfg->firstPointNode;
    while (livePointNode && livePointNode->value) {
        createLife(g, livePointNode->value->x, livePointNode->value->y);
        livePointNode = livePointNode->next;
    }

    errorFlag e = runLife(g, cfg->maxTurns);
    releaseGrid(g);

    return e;
}

errorFlag parseNumberOption(int *toSet, char *arg, char optionChar) {
    if (!arg || *(arg++) != '-') return FAILED;
    if (!arg || *(arg++) != optionChar) return FAILED;
    if (!arg || *(arg++) != '=') return FAILED;

    if (!arg) return FAILED;
    *toSet = atoi(arg);
    if (*toSet == 0 && *arg != '0' && ++arg != NULL) return FAILED;

    return SUCCESS;    
}

errorFlag parseMaxTurns(config *cfg, char *arg) {
    parseNumberOption(&(cfg->maxTurns), arg, 't');
    return SUCCESS;    
}

errorFlag parseWidth(config *cfg, char *arg) {
    parseNumberOption(&(cfg->width), arg, 'w');
    return SUCCESS;    
}

errorFlag parseHeight(config *cfg, char *arg) {
    parseNumberOption(&(cfg->height), arg, 'h');
    return SUCCESS;    
}

point *parsePoint(char* toParse) {
    if (!toParse) return NULL;
    char *strtokState;
    char *pointPart = strtok_r(toParse, ",", &strtokState);
    if (!pointPart) return NULL;
    int x = atoi(pointPart);
    if (x == 0 && *pointPart != '0' && ++pointPart != NULL) return FAILED;
    pointPart = strtok_r(NULL, ",", &strtokState);
    if (!pointPart) return NULL;
    int y = atoi(pointPart);
    if (y == 0 && *pointPart != '0' && ++pointPart != NULL) return FAILED;
    if (strtok_r(NULL, ",", &strtokState)) return NULL;

    return createPoint(x, y);
}

errorFlag parsePoints(config *cfg, char *arg) {
    if (!arg || *(arg++) != '-') return FAILED;
    if (!arg || *(arg++) != 'p') return FAILED;
    if (!arg || *(arg++) != '=') return FAILED;
    pointNode* current = NULL;
    char* strtokState;
    char* pointToParse = strtok_r(arg, "#", &strtokState);
    for (bool first = true; pointToParse; first = false) {
        point *p = parsePoint(pointToParse);
        if (!p) return FAILED;
        
        if (!first) {
            current->next = createPointNode(p, NULL);
            current = current->next;
        } else {
            current =  createPointNode(p, NULL);
        }
        if (first) cfg->firstPointNode = current;
        pointToParse = strtok_r(NULL, "#", &strtokState);
    }
    if (!cfg->firstPointNode) return FAILED;

    return SUCCESS;
}

errorFlag parseOption(config *cfg, char *arg) {
    if (!arg || arg[0] != '-') return FAILED;
    char option = arg[1];
    if (!option) return FAILED;
    switch (option) {
        case 't': return parseMaxTurns(cfg, arg);
        case 'w': return parseWidth(cfg, arg);
        case 'h': return parseHeight(cfg, arg);
        case 'p': return parsePoints(cfg, arg);
        default: return FAILED;
    }
}

errorFlag parseArgs(config *cfg, int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] == '-') {
            if (parseOption(cfg, arg) != SUCCESS) return FAILED;
        } else {
            return FAILED;
        }
    }
    return SUCCESS;
}

void main(int argc, char** argv) {
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
