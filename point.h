#ifndef _POINT_H_
#define _POINT_H_

typedef struct point { int x, y; } point;
point *createPoint(int x, int y);
void releasePoint(point *p);

typedef struct pointNode {
    point *value;
    struct pointNode *next;
} pointNode;
pointNode *createPointNode(point *p, pointNode *next);
void releasePointNode(pointNode *pn);
pointNode *copyPointNode(pointNode *pn);

#endif
