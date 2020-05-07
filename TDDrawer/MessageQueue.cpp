#include "MessageQueue.hpp"
#include <Windows.h>

void MyCreateThread(ThreadHandle * handle, lp_start_fun lpStart) {
	*handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)lpStart, nullptr, 1, 0);
	ResumeThread(*handle);
}