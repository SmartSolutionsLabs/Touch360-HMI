#ifndef MOTOR_INC
#define MOTOR_INC

#include "Thread.hpp"
#include "Commodity.hpp"

void IRAM_ATTR interruptMotorSecondHand(void* arg);

/**
 * The single motor for controlling it.
 */
class Motor : public Thread {
	public:
		enum Status {
			OFF,
			RUNNING,
			PAUSED,
			HALTED,
			FINISHED
		};

	protected:
		static Motor * motor;
		Motor();
		Motor(const char * name);

		Commodity paperUpStatus;
		Commodity paperDownStatus;

		unsigned int angularVelocity;
		unsigned int maxSpinsQuantity;
		volatile unsigned int currentSpinsQuantity;

		Status status;

	public:
		esp_timer_handle_t secondHandTimer = nullptr;

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

		void resetCurrentSpinsQuantity();

		unsigned int getAngularVelocity() const;
		unsigned int getMaxSpinsQuantity() const;
		unsigned int getCurrentSpinsQuantity() const;

		void incrementCurrentSpinsQuantity();

		unsigned int incrementAngularVelocity();

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
