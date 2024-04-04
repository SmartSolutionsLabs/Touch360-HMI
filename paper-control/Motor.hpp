#ifndef MOTOR_INC
#define MOTOR_INC

#include "Thread.hpp"
#include "Commodity.hpp"

/**
 * The single motor for controlling it.
 */
class Motor : public Thread {
	protected:
		static Motor * motor;
		Motor();
		Motor(const char * name);

		Commodity paperUpStatus;
		Commodity paperDownStatus;

		unsigned int angularVelocity;
		unsigned int maxSpinsQuantity;
		unsigned int remainingSpinsQuantity;

		enum Status {
			OFF,
			RUNNING,
			PAUSED,
			HALTED
		} status;

	public:
		// For singleton
		static Motor * getInstance();
		Motor(Motor &other) = delete;
		void operator=(const Motor &) = delete;

		void run(void* data);

		void parseIncome(void * data);

		/**
		 * Only can change it when motor is not running.
		 */
		void setMaxSpinsQuantity(unsigned int);

		unsigned int getAngularVelocity() const;
		unsigned int getMaxSpinsQuantity() const;
		unsigned int getRemainingSpinsQuantity() const;

		void decrementSpinsQuantity();

		/**
		 * Halt the motor as an emergency.
		 */
		void halt();

		/**
		 * Alternating running - paused.
		 */
		void toggleStatus();

		Status getStatus() const;

		Commodity getPaperUpStatus() const;
		Commodity getPaperDownStatus() const;
};

#endif
