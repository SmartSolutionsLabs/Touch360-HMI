#ifndef MOTOR_INC
#define MOTOR_INC

#include <Publisher.hpp>
#include <Observer.hpp>

#include "Commodity.hpp"

#define PIN_PAPER_UP 4
#define PIN_PAPER_DOWN 5
#define PIN_SPIN 6
#define PIN_TEST 7

#define PIN_MOTOR 0
#define PIN_ELECTROVALVE 2

#define MAX_MOTOR_VELOCITY 1200

#include "Status.hpp"

void IRAM_ATTR interruptMotorSecondHand(void* arg);

/**
 * The single motor for controlling it.
 */
class Motor : public Publisher, public Observer {
	protected:
		Commodity paperUpStatus;
		Commodity paperDownStatus;

		volatile int maxAngularVelocity;
		volatile int angularVelocity;
		unsigned int maxSpinsQuantity;
		volatile unsigned int currentSpinsQuantity;
		volatile Status status;

	public:
		Motor(const char * name, int taskCore = 1);

		esp_timer_handle_t secondHandTimer = nullptr;

		void connect(void * data) override;

		void run(void* data) override;

		void update() override;

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
		void halt(Status status = HALTED);

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
