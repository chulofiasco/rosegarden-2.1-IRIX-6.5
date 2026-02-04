
/* Music Editor            */
/* Linked List Functions   */
/* for Doubly Linked Lists */
/* Chris Cannam, Jan 1994  */

#include <stdio.h>
#include <stdlib.h>

#include "Debug.h"
#include "Lists.h"

List tmp__List;			/* for warnings stuff */


/* Ground rules: a List is */
/* any element in the list */
/* and when these return   */
/* Lists, they may return  */
/* any element, not only   */
/* the first.              */


/* Create a new one-elt list of a certain size -- probably   */
/* only really useful from other `derived' list-constructors */

List LIST_NewList(size_t size)
{
  List list;
  Begin("LIST_NewList");

  list = (List)malloc(size);

  if (!list) {
    fprintf(stderr,"List Module: Cannot perform malloc(%d)\n", (int)size);
    exit(1);
  }

  list->next = list->prev = NULL;
  Return(list);
}


/* Find the first or last element in the current list */

List LIST_First(List elt)
{
  Begin("LIST_First");

  if (!elt) Return(elt);
  while (Prev(elt)) elt = Prev(elt);

  Return(elt);
}

List LIST_Last(List elt)
{
  Begin("LIST_Last");

  if (!elt) Return(elt);
  while (Next(elt)) elt = Next(elt);

  Return(elt);
}


int LIST_Length(const List list)
{
  int  n;
  List current;
  Begin("LIST_Length");

  if (!list) Return(0);

  for (n = 0, current = list; current; current = Prev(current), ++n);
  for (current = Next(list);  current; current = Next(current), ++n);

  /*
   This function used to be:
     for (n = 0, current = First(list); current; current = current->next, ++n);
   but the new version is faster, only checking each elt once.
  */

  Return(n);
}


/* Free a whole list, given any element in it */

void LIST_DestroyList(List list)  /* strangely enough, this could be a const */
{
  List next, current;
  Begin("LIST_DestroyList");

  current = First(list);

  while (current) {
    next = Next(current);
    free(current);
    current = next;
  }
}


/* Destructive append; again, pointers may be anywhere in list. */
/* Aims to return an element as near to the end of the list as  */
/* possible, to optimize for loops in which you're adding elts  */
/* one at a time to the ends of lists.  This means it's not so  */
/* good when used for `cons', but you can always use Insert     */
/* (which is optimized the other way). (Using Insert takes more */
/* care, of course, because it matters which element you pass.) */

List LIST_Nconc(List a, List b)
{
  List last;
  List first;
  Begin("LIST_Nconc");

  last  = a ? Last(a)  : a;
  first = b ? First(b) : b;

  if (last)   last->next = first;
  if (first) first->prev = last;

  Return(b ? b : last);
}


/* Insert a before b; this is append where b is a whole list, or */
/* insert where b is an element down within a list somewhere.    */
/* Returns a pointer to the first elt of a, or if a was NULL, to */
/* the first elt of b.                                           */

List LIST_Insert(List a, List b)
{
  List aa;
  List az;
  List bp;
  Begin("LIST_Insert");
  
  if (!b) Return(First(a));
  if (!a) Return(First(b));
  if (!b->prev) {
    (void)(Nconc(a, b));
    Return(First(a));
  }

  az = Last(a);
  aa = First(a);
  bp = b->prev;

  az->next = b;
  b ->prev = az;
  bp->next = aa;
  aa->prev = bp;

  Return(aa);
}


/* Remove one element from a list, return the following element */
/* (so that you can repeatedly call Remove in a loop to remove  */
/* whole sublists)                                              */

List LIST_Remove(List a)
{
  List ret = NULL;
  Begin("LIST_Remove");

  if (!a) Return(a);

  if (Prev(a)) { ret = Prev(a); ret->next = Next(a); }
  if (Next(a)) { ret = Next(a); ret->prev = Prev(a); }

  free(a);
  Return(ret);
}


/* Destructive reverse. */

List LIST_Reverse(List a)
{
  List curr;
  List prev;
  Begin("LIST_Reverse");

  curr = First(a);

  while(curr) {

    prev       = Prev(curr);
    curr->prev = Next(curr);
    curr->next = prev;
    curr       = Prev(curr);
  }

  Return(a);
}


/* Divide list in two */

List LIST_Split(List a)
{
  List b = NULL;
  Begin("LIST_Split");

  if (a) {
      b = Prev(a);
      a->prev = NULL;
  }
  if (b) b->next = NULL;

  Return(b);
}


