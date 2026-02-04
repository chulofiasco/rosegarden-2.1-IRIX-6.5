/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	BHeap.c

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

#include <stdlib.h>

#include <MidiErrorHandler.h>
#include <MidiBHeap.h>

#include <Debug.h>

/*************************************/
/* Fundamental Binary Heap Operators */
/*************************************/


/*************************/
/* Constructor function. */
/*************************/


BinaryHeap CreateBHeap(int Size, OrderingFn Func)
{
BinaryHeap NewHeap;

BEGIN("CreateBHeap");

	NewHeap = (BinaryHeap)malloc(sizeof(BinaryHeapStruct));

	if (!NewHeap) RETURN_PTR(NULL);

	NewHeap->Size    = Size;
	NewHeap->NumElts = 0;
	NewHeap->Lessp	 = Func;

	NewHeap->Values = (void **)malloc((Size + 1) * sizeof(void *));

	if (!NewHeap->Values)
	{
		free(NewHeap);
		RETURN_PTR(NULL);
	}

RETURN_PTR(NewHeap);
}


/************************/
/* Destructor Function. */
/************************/

void DestroyBHeap(BinaryHeap Heap)
{
BEGIN("DestroyBHeap");

	if (!Heap) END;

	if (Heap->Values) free(Heap->Values);

	free(Heap);

END;
}


/*************************************************************************/
/* SiftUp - Sift the specified element up the heap as far as it will go. */
/*************************************************************************/
	 
void SiftUp  (BinaryHeap Heap, int Element)
{
void *Temp;

BEGIN("SiftUp");

	while(Element != 1 && !(Heap->Lessp(Value(Heap, Parent(Element)), Value(Heap, Element))))
	{
		Temp = Value(Heap, Parent(Element));
		Value(Heap, Parent(Element)) = Value(Heap, Element);
		Value(Heap, Element) = Temp;

		Element = Parent(Element);
	}
END;
}




/************************************************************************/
/* SiftDown - Sift the root element down the heap as far as it will go. */
/************************************************************************/

void SiftDown(BinaryHeap Heap)
{
int Element, Child;
void *Temp;

BEGIN("SiftDown");

	Element = Root;

	while ((Child = LeftChild(Element)) <= HeapSize(Heap))
	{
		if (RightChild(Element) <= HeapSize(Heap) &&
		    Heap->Lessp(Value(Heap, RightChild(Element)), Value(Heap, Child)))
		{
			Child = RightChild(Element);
		}

		if (Heap->Lessp(Value(Heap, Element), Value(Heap, Child))) break;

		Temp                 = Value(Heap, Element);
		Value(Heap, Element) = Value(Heap, Child);
		Value(Heap, Child)   = Temp;
		Element              = Child;
	}

END;
}

		

/***********************************************/
/* Functions provided to make the heap useful. */
/***********************************************/


/**********************************************************************/
/* Functions to enable a binary heap to function as a priority queue. */
/**********************************************************************/

void  BHeapInsert(BinaryHeap Heap, void *NewElement)
{
BEGIN("Insert");

	if (HeapSize(Heap) >= HeapMaxSize(Heap)) END;

	++HeapSize(Heap);

	Value(Heap, HeapSize(Heap)) = NewElement;
	SiftUp(Heap, HeapSize(Heap));

END;
}


void *ExtractMin(BinaryHeap Heap)
{
void *ReturnValue;

BEGIN("ExtractMin");

	if (HeapSize(Heap) < 1) RETURN_PTR(NULL);

	ReturnValue = Value(Heap, Root);

	Value(Heap, Root) = Value(Heap, HeapSize(Heap));

	--HeapSize(Heap);

	if (HeapSize(Heap) > 0) SiftDown(Heap);

RETURN_PTR(ReturnValue);
}
