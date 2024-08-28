#ifndef CONTROL_INC
#define CONTROL_INC

#include "Status.hpp"

#include <Queue.h>
#include <Application.hpp>

#include <Preferences.h>

enum GloogerEvent {
	LOG,
	STOCK
};

struct Gloog {
	GloogerEvent type;
	time_t unixtime;
	Status status;
	unsigned int data; // optional. Generally spins quantity
};

struct Roll {
	unsigned int maxSpinsQuantity;
	Queue<unsigned int> history;
	String name;
	Preferences preferences;
};

class Control : public Application {
	protected:
		Roll rolls[3];

		Preferences preferences;

	public:
		Control();

		enum View {
			HOME,
			CONFIGURATION,
			HISTORY,
			CREDITS
		} view;

		/**
		 * Queue as display instructions buffer.
		 */
		Queue<String> messagesQueue;

		/**
		 * Queue to Glooger.
		 */
		Queue<Gloog> gloogerQueue;

		void setRollQuantity(unsigned int typeIndex, unsigned int position, unsigned int value);

		unsigned int getRollQuantity(unsigned int typeIndex) const;

		void setRollName(unsigned int typeIndex, const char * name);

		String getRollName(unsigned int typeIndex) const;

		void saveMaxVelocity(int maxVelocity);

		int getMaxVelocity();

		// Set and save credentials of network
		void setNetworkPassword(String networkPassword);
		void setNetworkSsid(String networkSsid);

		void addGloog(GloogerEvent event, Status status, unsigned int data = 0);

		void processMessage(unsigned char * message, size_t length, bool printable) override;

		void initializeModulesPointerArray() override;
};

#endif
