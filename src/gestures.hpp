
#ifndef GESTURES_H
#define GESTURES_H

// shapeListElem contains information about the position of a recognized shape and a pointer to the next in the list
typedef struct shapeListElem
{
	int x;
	int y;
	shapeListElem *next;
} shapeListElem;

void createEvents( void ); // uses data provided by the shape-recogition to generate a list of Events

#endif
