#ifndef _PLAYLISTQUEUE_H_
#define _PLAYLISTQUEUE_H_ _PLAYLISTQUEUE_H_

#include <QList>
#include <QTableWidgetItem>

const int MAX_QUEUE_SIZE = 16;

class PlayListQueue
{
	private:
		QList<const QTableWidgetItem*> list;
	public:
		const QTableWidgetItem* dequeue() {
			return list.isEmpty() ? NULL : list.takeLast();
		}

		void enqueue(const QTableWidgetItem* newItem);

		inline bool contains(const QTableWidgetItem* item) const {
			return list.contains(item);
		}

		inline bool isEmpty() const {
			return list.isEmpty();
		}

};

#endif // _PLAYLISTQUEUE_H_
