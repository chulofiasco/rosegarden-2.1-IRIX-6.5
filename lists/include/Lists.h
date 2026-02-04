
#ifndef _DOUBLY_LINKED_LISTS_
#define _DOUBLY_LINKED_LISTS_


/*
                 Doubly-linked lists, Chris Cannam 1994
                 ======================================

   The ListElement is supposed to  be an overlapping generic structure
   on  top of your own  specialised  "List-of-X" type.  Define them as
   in the example--

         typedef struct _IntListElement {
	   ListElement generic;
	   int         item;
	 } IntListElement, *IntList;

   You  should then define a   constructor  function (or macro)  which
   takes an int and returns an IntList--

         IntList NewIntList(int x)
	 {
	   IntList list;

	   list = (IntList)NewList(sizeof(IntListElement));
	   list->item = x;

	   return list;
	 }

   You're then ready to go.  These generic  List functions can be used
   by casting your  specialised list pointers to the  List type in the
   call, and  casting  returned values  back again.  This  header file
   includes  macros  which  obviate  the  need  to cast the arguments,
   though you'll still need to cast the return values back.  You might
   consider building some other specialist  functions, such as `cons',
   but that's up to you.

   Note that all the List functions assume that a List is defined by a
   pointer to any element  in it.  You  can as freely pass  Last(X) to
   any of  these functions  as X or  First(X),  and except in  special
   cases where a particular element is called for (such as Remove), it
   won't make a  blind bit  of  difference.   Similarly, there is   no
   reason why a returned  list pointer should  point to the start of a
   list.
*/


#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#ifndef LIST_WARNINGS
#define LIST_WARNINGS
#endif
#endif
#ifndef __STDC__
#undef LIST_WARNINGS
#endif


typedef struct _ListElement {
  struct _ListElement *next;
  struct _ListElement *prev;
} ListElement, *List;


extern List    LIST_NewList     (size_t s);
extern List    LIST_First       (List a);
extern List    LIST_Last        (List a);
extern int     LIST_Length      (const List a);
extern void    LIST_DestroyList (List a);    /* traverses in forwards order */
extern List    LIST_Nconc       (List a, List b);
extern List    LIST_Insert      (List a, List b);
				             /* inserts a before element b  */
extern List    LIST_Remove      (List a);
				             /* removes elt a, returns next */
                                             /*  (prev if there is no next) */
extern List    LIST_Reverse     (List a);
extern List    LIST_Split       (List a);
				             /* breaks list in two before a */
				             /* and returns first half (you */
				             /* should already have second, */
				             /* 'cos it was the passed arg) */


/* Macros to move list pointers both ways -- */
/* both return lists which are essentially   */
/* the same as the passed one, but described */
/* differently                               */

#ifdef LIST_WARNINGS

extern List tmp__List;

#define cast__List(x,y) (((long)(tmp__List=((List)(y))) < 0xffff) ? \
			 (fprintf(stderr,"Warning: %s[%s=%p] at %s/%d\n", \
                                  #x, #y, tmp__List, __FILE__, __LINE__), \
			 tmp__List) : tmp__List)

#define Prev(x)   (((long)(tmp__List=((List)(x))) < 0xffff) ? \
		   (fprintf(stderr,"Error: Prev(%s=%p) at %s/%d\n", \
                            #x, tmp__List, __FILE__, __LINE__), \
		   (tmp__List ? tmp__List->prev : tmp__List)) : \
		   (tmp__List->prev ? cast__List(val, tmp__List->prev) : 0))

#define Next(x)   (((long)(tmp__List=((List)(x))) < 0xffff) ? \
		   (fprintf(stderr,"Error: Next(%s=%p) at %s/%d\n", \
                            #x, tmp__List, __FILE__, __LINE__), \
		   (tmp__List ? tmp__List->next : tmp__List)) : \
		   (tmp__List->next ? cast__List(val, tmp__List->next) : 0))

/* no, maybe I *don't* want all those warnings for Nconc and stuff */
#undef cast__List
#define cast__List(x,y) ((List)(y))

#else

#define cast__List(x,y) ((List)(y))

#define Prev(x)   (((List)(x))->prev)
#define Next(x)   (((List)(x))->next)

#endif


/* Macros to do some of the casting for you. */
/* Obviously if you use these you'll have to */
/* be particularly careful that your types   */
/* are actually the right ones               */

#define NewList(x)   LIST_NewList((size_t)(x))
#define DestroyList(x) LIST_DestroyList(cast__List(DestroyList,(x)))
#define Nconc(x,y)   LIST_Nconc(cast__List(Nconc,(x)),cast__List(Nconc,(y)))
#define First(x)     LIST_First(cast__List(First,(x)))
#define Last(x)      LIST_Last(cast__List(Last,(x)))
#define Length(x)    LIST_Length(cast__List(Length,(x)))
#define Insert(x,y)  LIST_Insert(cast__List(Insert,(x)),cast__List(Insert,(y)))
#define Remove(x)    LIST_Remove(cast__List(Remove,(x)))
#define Reverse(x)   LIST_Reverse(cast__List(Reverse,(x)))
#define Split(x)     LIST_Split(cast__List(Split,(x)))


#endif /* _DOUBLY_LINKED_LISTS_ */

