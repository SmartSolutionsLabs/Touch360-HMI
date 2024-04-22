#ifndef THREAD_INC
#define THREAD_INC

#include <Arduino.h>

#include "Control.hpp"
#include "esp_task_wdt.h"

class Thread {
	protected:
		TaskHandle_t taskHandler = nullptr;
		const int taskCore = 0;
		const int taskPriority = 1;
		const char * name;

		Control * control;

	public:
		Thread(const char * name);

		static void runTask(void* data);

		void start();

		void stop();

		virtual void parseIncome(void * data) = 0;

		virtual void run(void* data) = 0; // Make run pure virtual
};

#endif
