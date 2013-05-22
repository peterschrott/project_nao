


#ifndef GESTURES_H
#define GESTURES_H

typedef struct shapeListElem; // might be needed by the next-pointer ...
// shapeListElem contains information about the position of a recognized shape and a pointer to the next in the list
typedef struct shapeListElem {
	int x,
	int y,
	shapeListElem next = NULL; // TODO: verify if this kind of initialization is allowed by C/C++
}

void createEvents( void ); // uses data provided by the shape-recogition to generate a list of Events

#endif
