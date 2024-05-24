#ifndef CONTROL_INC
#define CONTROL_INC

#include "Status.hpp"
#include "Queue.h"

#include <Preferences.h>

struct Gloog {
	unsigned int type;
	time_t unixtime;
	unsigned int status;
	unsigned int data; // optional. Generally spins quantity
};

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

		enum GloogerEvent {
			LOG,
			STOCK
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
		 * Queue to Glooger.
		 */
		Queue<Gloog> gloogerQueue;

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

		void addGloog(unsigned int type, GloogerEvent event, Status status, unsigned int data);

	private:
		DisplayStatus displayStatus;
};

#endif
