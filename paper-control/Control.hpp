#ifndef CONTROL_INC
#define CONTROL_INC

#include "Queue.h"

#include <Preferences.h>

struct Roll {
	unsigned int maxSpinsQuantity;
	Queue<unsigned int> history;
	String name;
	Preferences preferences;
};

class Control {
	protected:
		static Control * control;
		Control();

		Roll rolls[3];

	public:
		enum DisplayStatus {
			SENDING,
			RECEIVING
		};

		// For singleton
		static Control * getInstance();
		Control(Control &other) = delete;
		void operator=(const Control &) = delete;

		/**
		 * Queue as display instructions buffer.
		 */
		Queue<String> messagesQueue;

		/**
		 * Set instruction type for sending in display.
		 */
		void setDisplaySending();

		/**
		 * Set instruction type for receiving in display.
		 */
		void setDisplayReceiving();

		DisplayStatus getDisplayStatus() const;

		void setRollQuantity(unsigned int typeIndex, unsigned int position, unsigned int value);

		void setRollName(unsigned int typeIndex, const char * name);

	private:
		DisplayStatus displayStatus;
};

#endif
