#ifndef WATCHER_INC
#define WATCHER_INC

#include "Thread.hpp"

class Watcher : public Thread {
	private:

	public:
		Watcher(const char * name);

		void run(void* data);

		void parseIncome(void * data);
};

#endif
