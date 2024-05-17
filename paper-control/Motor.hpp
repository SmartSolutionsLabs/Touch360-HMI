#ifndef MOTOR_INC
#define MOTOR_INC

#include "Thread.hpp"
#include "Commodity.hpp"

#define PIN_PAPER_UP 4
#define PIN_PAPER_DOWN 5
#define PIN_SPIN 6
#define PIN_TEST 7

#define PIN_MOTOR 0
#define PIN_ELECTROVALVE 2

#define MAX_MOTOR_VELOCITY 1200

void IRAM_ATTR interruptMotorSecondHand(void* arg);

/**
 * The single motor for controlling it.
 */
class Motor : public Thread {
	public:
		enum Status {
			OFF,
			TEST,
			RUNNING,
			RUNNING_WITH_BREAK,
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

		volatile int maxAngularVelocity;
		volatile int angularVelocity;
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

		int getAngularVelocity() const;
		unsigned int getMaxSpinsQuantity() const;
		unsigned int getCurrentSpinsQuantity() const;

		void incrementCurrentSpinsQuantity();

		int incrementAngularVelocity();
		int decrementAngularVelocity();
		void setMaxAngularVelocity(int maxAngularVelocity);
		int getMaxAngularVelocity() const;

		/**
		 * Halt the motor as an emergency.
		 */
		void halt();

		/**
		 * Finished.
		 */
		void stop();

		/**
		 * Alternating running - paused.
		 */
		void toggleStatus();

		Status getStatus() const;

		Commodity getPaperUpStatus() const;
		Commodity getPaperDownStatus() const;
};

#endif
