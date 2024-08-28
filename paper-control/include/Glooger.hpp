#ifndef GLOOGER_INC
#define GLOOGER_INC

#include <Observer.hpp>
#include <Thread.hpp>

/**
 * Send data to Google Drive.
 */
class Glooger : public Thread, public Observer {
	private:

	public:
		Glooger(const char * name);

		void run(void* data) override;

		void update() override;
};

#endif
