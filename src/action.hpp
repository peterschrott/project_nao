
#ifndef ACTION_H
#define ACTION_H

typedef struct {
	int id; // what kind of event happened
	char *name; // string representation of event (for logging ...)
	int *data; // could be used to handle data like position of a recognized sound
} naoEvent;

#ifdef PC
// here the macros for pc

#define HANDLE_EVENT(x) printf( x->name );


#else
// here the macros for nao

#define HANDLE_EVENT(x) handleEvent( x );

#endif

// this function only needs to be implemented on nao
void handleEvent( naoEvent e );

#endif
