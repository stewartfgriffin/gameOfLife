/*
 * Provide point and pointNode types.
 *
 * Point is a simple x,y coordinate.
 *
 * PointNode is a linked list node that points to a point and the next
 * point node.
 */
#include <stdlib.h>
#include "point.h"

point *createPoint(int x, int y) {
    point *p = malloc(sizeof(point));
    if (!p) return NULL;
    p->x = x;
    p->y = y;
    return p;
}

void releasePoint(point *p){
    free(p);
}

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
