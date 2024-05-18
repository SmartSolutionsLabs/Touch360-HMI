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

		Preferences preferences;

	public:
		enum DisplayStatus {
			SENDING,
			RECEIVING
		};

		enum View {
			HOME,
			CONFIGURATION,
			HISTORY,
			CREDITS
		} view;

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

		unsigned int getRollQuantity(unsigned int typeIndex) const;

		void setRollName(unsigned int typeIndex, const char * name);

		String getRollName(unsigned int typeIndex) const;

		void saveMaxVelocity(int maxVelocity);

		int getMaxVelocity();

		// Set and save credentials of network
		void setNetworkPassword(String networkPassword);
		void setNetworkSsid(String networkSsid);

	private:
		DisplayStatus displayStatus;
};

#endif
