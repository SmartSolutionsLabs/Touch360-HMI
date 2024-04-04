#ifndef CONTROL_INC
#define CONTROL_INC

#include "Queue.h"

class Control {
	protected:
		static Control * control;
		Control();

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

	private:
		DisplayStatus displayStatus;
};

#endif
