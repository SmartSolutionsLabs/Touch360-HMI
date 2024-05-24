#ifndef GLOOGER_INC
#define GLOOGER_INC

#include "Thread.hpp"

/**
 * Send data to Google Drive.
 */
class Glooger : public Thread {
	private:

	public:
		Glooger(const char * name);

		void run(void* data);

		void parseIncome(void * data);
};

#endif
