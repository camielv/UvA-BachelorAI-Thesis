#pragma once

#include <windows.h>
#include <queue>

template <class T>
class slam_queue
{
public:
	slam_queue()
	{
		event_pushed = CreateEvent(NULL, false, false, NULL /*(LPTSTR) "SLAM_QUEUE_PUSHED"*/);
		event_empty = CreateEvent(NULL, false, true, NULL /*(LPTSTR) "SLAM_QUEUE_EMPTY"*/);

		hMutex = CreateMutex(NULL, FALSE, NULL);
	}

	~slam_queue()
	{
	}

	void push(const T &t)
	{
		q.push(t);
		ResetEvent(event_empty);
		SetEvent(event_pushed);
	}

	T& front()
	{
		return q.front();
	}

	void pop()
	{
		q.pop();
	}

	bool empty()
	{
		if (q.empty())
		{
			ResetEvent(event_pushed);
			SetEvent(event_empty);
			return true;
		}
		else
		{
			ResetEvent(event_empty);
			return false;
		}
	}

	void wait_until_filled(DWORD timeout)
	{
		if (q.empty())
			WaitForSingleObject(event_pushed, timeout);
	}

	void wait_until_empty(DWORD timeout)
	{
		if (!q.empty())
			WaitForSingleObject(event_empty, timeout);
	}

	void lock()
	{
		WaitForSingleObject(hMutex, 2000);
	}

	void release()
	{
		ReleaseMutex(hMutex);
	}

	std::queue<T> q;


private:

	HANDLE event_pushed;
	HANDLE event_empty;
	HANDLE hMutex;
};

