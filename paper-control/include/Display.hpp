#ifndef DISPLAY_INC
#define DISPLAY_INC

#include <Publisher.hpp>

class Display : public Publisher {
	public:
		Display(const char * name, int taskCore = 1);

		void connect(void * data) override;

		void run(void* data) override;

		void parseIncome(unsigned char * data, size_t length);
};

#endif
