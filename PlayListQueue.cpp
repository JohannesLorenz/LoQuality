#include "PlayListQueue.h"

void PlayListQueue::enqueue(const QTableWidgetItem* newItem)
{
	if(newItem)
	{
		if( list.size() == MAX_QUEUE_SIZE )
		 list.pop_front();
		list.append(newItem);
	}
}
