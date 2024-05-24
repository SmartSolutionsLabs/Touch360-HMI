#ifndef DISPLAY_INC
#define DISPLAY_INC

#include "Thread.hpp"

class Display : public Thread {
	private:

	public:
		Display(const char * name);

		void run(void* data);

		void parseIncome(void * data);
};

#endif
