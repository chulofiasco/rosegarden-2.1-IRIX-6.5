/* 
 * $Source: /u/durian/cvs/tclm2/rb/list.h,v $
 * $Revision: 1.3 $
 * $Date: 1995/09/06 17:53:43 $
 * $Author: durian $
 */

/* This is the header file for the list manipulation routines in list.c.
 * Any struct can be turned into a list as long as its first two fields are
 * flink and blink. */

typedef struct list {
  struct list *flink;
  struct list *blink;
} *List;

/* Nil, first, next, and prev are macro expansions for list traversal 
 * primitives. */

#ifndef nil
#define nil(l) (l)
#endif
#define first(l) (l->flink)
#define last(l) (l->blink)
#define next(n) (n->flink)
#define prev(n) (n->blink)

#define mklist(t) ((t *) make_list (sizeof(t)))

/* These are the routines for manipluating lists */

/* void insert(node list);     Inserts a node to the end of a list */
/* void delete_item(node);     Deletes an arbitrary node */
/* List make_list(node_size);  Creates a new list */
/* List get_node(list);        Allocates a node to be inserted into the list */
/* void free_node(node, list); Deallocates a node from the list */

#ifndef __P
#if defined(__STDC__) || defined(__cplusplus)
#define __P(protos)	protos
#else 
#define __P(protos)	()
#endif
#endif

extern void insert __P((List, List));
extern void delete_item __P((List));
extern List make_list __P((int));
extern List get_node __P((List));
extern void free_node __P((List, List));
