///*
//    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
//    All rights reserved
//*/

//#ifndef INC_FREERTOS_H
//	#error FreeRTOS.h must be included before list.h
//#endif

//#ifndef LIST_H
//#define LIST_H


//#ifndef configLIST_VOLATILE
//	#define configLIST_VOLATILE
//#endif /* configSUPPORT_CROSS_MODULE_OPTIMISATION */

//#ifdef __cplusplus
//extern "C" {
//#endif


/////*
//// * Definition of the only type of object that a list can contain.
//// */
////struct xLIST_ITEM
////{
//////	listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE			/*< Set to a known value if configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
////	configLIST_VOLATILE TickType_t xItemValue;			/*< The value being listed.  In most cases this is used to sort the list in descending order. */
////	struct xLIST_ITEM * configLIST_VOLATILE pxNext;		/*< Pointer to the next ListItem_t in the list. */
////	struct xLIST_ITEM * configLIST_VOLATILE pxPrevious;	/*< Pointer to the previous ListItem_t in the list. */
////	void * pvOwner;										/*< Pointer to the object (normally a TCB) that contains the list item.  There is therefore a two way link between the object containing the list item and the list item itself. */
////	void * configLIST_VOLATILE pvContainer;				/*< Pointer to the list in which this list item is placed (if any). */
//////	listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE			/*< Set to a known value if configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
////};
////typedef struct xLIST_ITEM ListItem_t;					/* For some reason lint wants this as two separate definitions. */

////struct xMINI_LIST_ITEM
////{
//////	listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE			/*< Set to a known value if configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
////	configLIST_VOLATILE TickType_t xItemValue;
////	struct xLIST_ITEM * configLIST_VOLATILE pxNext;
////	struct xLIST_ITEM * configLIST_VOLATILE pxPrevious;
////};
////typedef struct xMINI_LIST_ITEM MiniListItem_t;

/////*
//// * Definition of the type of queue used by the scheduler.
//// */
////typedef struct xLIST
////{
//////	listFIRST_LIST_INTEGRITY_CHECK_VALUE				/*< Set to a known value if configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
////	configLIST_VOLATILE UBaseType_t uxNumberOfItems;
////	ListItem_t * configLIST_VOLATILE pxIndex;			/*< Used to walk through the list.  Points to the last item returned by a call to listGET_OWNER_OF_NEXT_ENTRY (). */
////	MiniListItem_t xListEnd;							/*< List item that contains the maximum possible item value meaning it is always at the end of the list and is therefore used as a marker. */
//////	listSECOND_LIST_INTEGRITY_CHECK_VALUE				/*< Set to a known value if configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
////} List_t;

/////*
//// * Access macro to set the owner of a list item.  The owner of a list item
//// * is the object (usually a TCB) that contains the list item.
//// *
//// * \page listSET_LIST_ITEM_OWNER listSET_LIST_ITEM_OWNER
//// * \ingroup LinkedList
//// */
//////#define listSET_LIST_ITEM_OWNER( pxListItem, pxOwner )		( ( pxListItem )->pvOwner = ( void * ) ( pxOwner ) )

/////*
//// * Access macro to get the owner of a list item.  The owner of a list item
//// * is the object (usually a TCB) that contains the list item.
//// *
//// * \page listSET_LIST_ITEM_OWNER listSET_LIST_ITEM_OWNER
//// * \ingroup LinkedList
//// */
//////#define listGET_LIST_ITEM_OWNER( pxListItem )	( ( pxListItem )->pvOwner )

/////*
//// * Access macro to set the value of the list item.  In most cases the value is
//// * used to sort the list in descending order.
//// *
//// * \page listSET_LIST_ITEM_VALUE listSET_LIST_ITEM_VALUE
//// * \ingroup LinkedList
//// */
////#define listSET_LIST_ITEM_VALUE( pxListItem, xValue )	( ( pxListItem )->xItemValue = ( xValue ) )

/////*
//// * Access macro to retrieve the value of the list item.  The value can
//// * represent anything - for example the priority of a task, or the time at
//// * which a task should be unblocked.
//// *
//// * \page listGET_LIST_ITEM_VALUE listGET_LIST_ITEM_VALUE
//// * \ingroup LinkedList
//// */
////#define listGET_LIST_ITEM_VALUE( pxListItem )	( ( pxListItem )->xItemValue )


/////*
//// * Access macro to determine if a list contains any items.  The macro will
//// * only have the value true if the list is empty.
//// *
//// * \page listLIST_IS_EMPTY listLIST_IS_EMPTY
//// * \ingroup LinkedList
//// */
////#define listLIST_IS_EMPTY( pxList )	( ( BaseType_t ) ( ( pxList )->uxNumberOfItems == ( UBaseType_t ) 0 ) )

/////*
//// * Access macro to return the number of items in the list.
//// */
////#define listCURRENT_LIST_LENGTH( pxList )	( ( pxList )->uxNumberOfItems )

/////*
//// * Access function to obtain the owner of the next entry in a list.
//// *
//// * The list member pxIndex is used to walk through a list.  Calling
//// * listGET_OWNER_OF_NEXT_ENTRY increments pxIndex to the next item in the list
//// * and returns that entry's pxOwner parameter.  Using multiple calls to this
//// * function it is therefore possible to move through every item contained in
//// * a list.
//// *
//// * The pxOwner parameter of a list item is a pointer to the object that owns
//// * the list item.  In the scheduler this is normally a task control block.
//// * The pxOwner parameter effectively creates a two way link between the list
//// * item and its owner.
//// *
//// * @param pxTCB pxTCB is set to the address of the owner of the next list item.
//// * @param pxList The list from which the next item owner is to be returned.
//// *
//// * \page listGET_OWNER_OF_NEXT_ENTRY listGET_OWNER_OF_NEXT_ENTRY
//// * \ingroup LinkedList
//// */
////#define listGET_OWNER_OF_NEXT_ENTRY( pxTCB, pxList )										\
////{																							\
////List_t * const pxConstList = ( pxList );													\
////	/* Increment the index to the next item and return the item, ensuring */				\
////	/* we don't return the marker used at the end of the list.  */							\
////	( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;							\
////	if( ( void * ) ( pxConstList )->pxIndex == ( void * ) &( ( pxConstList )->xListEnd ) )	\
////	{																						\
////		( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;						\
////	}																						\
////	( pxTCB ) = ( pxConstList )->pxIndex->pvOwner;											\
////}


/////*
//// * Access function to obtain the owner of the first entry in a list.  Lists
//// * are normally sorted in ascending item value order.
//// *
//// * This function returns the pxOwner member of the first item in the list.
//// * The pxOwner parameter of a list item is a pointer to the object that owns
//// * the list item.  In the scheduler this is normally a task control block.
//// * The pxOwner parameter effectively creates a two way link between the list
//// * item and its owner.
//// *
//// * @param pxList The list from which the owner of the head item is to be
//// * returned.
//// *
//// * \page listGET_OWNER_OF_HEAD_ENTRY listGET_OWNER_OF_HEAD_ENTRY
//// * \ingroup LinkedList
//// */
////#define listGET_OWNER_OF_HEAD_ENTRY( pxList )  ( (&( ( pxList )->xListEnd ))->pxNext->pvOwner )

/////*
//// * Return the list a list item is contained within (referenced from).
//// *
//// * @param pxListItem The list item being queried.
//// * @return A pointer to the List_t object that references the pxListItem
//// */
////#define listLIST_ITEM_CONTAINER( pxListItem ) ( ( pxListItem )->pvContainer )

/////*
//// * This provides a crude means of knowing if a list has been initialised, as
//// * pxList->xListEnd.xItemValue is set to portMAX_DELAY by the vListInitialise()
//// * function.
//// */
////#define listLIST_IS_INITIALISED( pxList ) ( ( pxList )->xListEnd.xItemValue == portMAX_DELAY )

/////*
//// * Must be called before a list is used!  This initialises all the members
//// * of the list structure and inserts the xListEnd item into the list as a
//// * marker to the back of the list.
//// *
//// * @param pxList Pointer to the list being initialised.
//// *
//// * \page vListInitialise vListInitialise
//// * \ingroup LinkedList
//// */
////void vListInitialise( List_t * const pxList ) ;


/////*
//// * Insert a list item into a list.  The item will be inserted into the list in
//// * a position determined by its item value (descending item value order).
//// *
//// * @param pxList The list into which the item is to be inserted.
//// *
//// * @param pxNewListItem The item that is to be placed in the list.
//// *
//// * \page vListInsert vListInsert
//// * \ingroup LinkedList
//// */
////void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem ) ;

/////*
//// * Insert a list item into a list.  The item will be inserted in a position
//// * such that it will be the last item within the list returned by multiple
//// * calls to listGET_OWNER_OF_NEXT_ENTRY.
//// *
//// * The list member pvIndex is used to walk through a list.  Calling
//// * listGET_OWNER_OF_NEXT_ENTRY increments pvIndex to the next item in the list.
//// * Placing an item in a list using vListInsertEnd effectively places the item
//// * in the list position pointed to by pvIndex.  This means that every other
//// * item within the list will be returned by listGET_OWNER_OF_NEXT_ENTRY before
//// * the pvIndex parameter again points to the item being inserted.
//// *
//// * @param pxList The list into which the item is to be inserted.
//// *
//// * @param pxNewListItem The list item to be inserted into the list.
//// *
//// * \page vListInsertEnd vListInsertEnd
//// * \ingroup LinkedList
//// */
////void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem ) ;

/////*
//// * Remove an item from a list.  The list item has a pointer to the list that
//// * it is in, so only the list item need be passed into the function.
//// *
//// * @param uxListRemove The item to be removed.  The item will remove itself from
//// * the list pointed to by it's pxContainer parameter.
//// *
//// * @return The number of items that remain in the list after the list item has
//// * been removed.
//// *
//// * \page uxListRemove uxListRemove
//// * \ingroup LinkedList
//// */
////UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove ) ;

//#ifdef __cplusplus
//}
//#endif

//#endif

