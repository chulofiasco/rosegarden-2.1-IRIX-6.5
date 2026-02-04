/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	BHeap.h

Description:	Implementation of Binary Heap data type. For details on the mechanisms
		underlying the Binary Heap, as well as complexity analysis, see the book
		"Programming Pearls" by Jon Bently.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	02/03/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/


/************************************************************************/
/* A generic ordering function prototype. The ordering function must be */
/* supplied when creating the heap, and must have the ability to place  */
/* a total ordering over the elements that are to be held in the heap.  */
/************************************************************************/

typedef int (*OrderingFn)(void *, void *);




/*************************************************************/
/* Binary heap definition. A binary heap has a maximum size, */
/* a record of the number of elements currently contained in */
/* the heap, an ordering function operating on the elements  */
/* of the heap, and an array of elements.		     */
/*************************************************************/

typedef struct
{
	int		Size;
	int		NumElts;
	OrderingFn	Lessp;
	void	      **Values;
}
BinaryHeapStruct, *BinaryHeap;



/*************************************/
/* Fundamental Binary Heap Operators */
/*************************************/

#define Root 				1
#define Value(HEAP, ELT)	HEAP->Values[ELT]
#define LeftChild(ELT)		(ELT << 1)
#define RightChild(ELT)		((ELT << 1) + 1)
#define Parent(ELT)		(ELT >> 1)
#define Null(HEAP, ELT)		((ELT < 1) || (ELT >> HEAP->NumElts))
#define HeapSize(HEAP)		HEAP->NumElts
#define HeapMaxSize(HEAP)	HEAP->Size

BinaryHeap CreateBHeap (int Size, OrderingFn Func);
void 	   DestroyBHeap(BinaryHeap Heap);

void 	   SiftUp  (BinaryHeap Heap, int Element);
void 	   SiftDown(BinaryHeap Heap);




/***********************************************/
/* Functions provided to make the heap useful. */
/***********************************************/

void  BHeapInsert(BinaryHeap Heap, void *NewElement);
void *ExtractMin(BinaryHeap Heap);
