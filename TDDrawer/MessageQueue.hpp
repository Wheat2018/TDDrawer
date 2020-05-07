#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>


typedef void* ThreadHandle;
#define LP_START_FUN(name_of_function) void name_of_function()
typedef void(*lp_start_fun)(void);

void MyCreateThread(ThreadHandle *handle, lp_start_fun lpStart);

template<typename T>
class MessageQueue {
public:
	MessageQueue() {}
	~MessageQueue() {}

	void Clear()
	{
		std::lock_guard<std::mutex> lk(mut);            // 1.全局加锁
		std::queue<T> empty;
		std::swap(empty, data_queue);
		cond.notify_all();
	}

	void push(const T& new_data)
	{
		std::lock_guard<std::mutex> lk(mut);            // 1.全局加锁
		data_queue.push(std::move(new_data));           // 2.push时独占锁
		cond.notify_one();
	}
	void wait_and_pop(T& val)
	{
		std::unique_lock<std::mutex> ulk(mut);                    // 3.全局加锁
		cond.wait(ulk, [this]() { return !data_queue.empty(); });  // 4.front 和 pop_front时独占锁
		val = std::move(data_queue.front());
		data_queue.pop();
	}
	bool empty()
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.empty();
	}
private:
	std::queue<T> data_queue;
	std::mutex mut;
	std::condition_variable cond;
};
