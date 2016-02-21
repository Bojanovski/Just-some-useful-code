//***************************************************************
// AsyncWorker.h by Bojan Lovrovic (C) 2016 All Rights Reserved.
//
// Abstract class to be used as a base for a worker class 
// that can quickly and efficiently perform certain task whenever
// it's being called.
//***************************************************************

#ifndef ASYNCWORKER_H
#define ASYNCWORKER_H

#include <mutex>
#include <thread>
#include <condition_variable>

class AsyncWorker
{
public:
	AsyncWorker();
	virtual ~AsyncWorker();

	// Assign a job that will be executed asynchronously.
	void Assign(void *data, size_t dataSize);

	// Wait for worker to finish it's assignment.
	void Join();

protected:
	void *mData;
	size_t mDataSize;

private:
	__declspec(dllexport) void WorkingProc();
	virtual void Work() = 0;

	std::mutex mLock;
	std::condition_variable mStartCheck;
	std::condition_variable mFinishCheck;
	std::thread mWorkingProcThread;
	bool mDoWork;
	bool mLoop;
};

#endif