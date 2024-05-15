#include "Display.hpp"
#include "Control.hpp"
#include "Motor.hpp"

Motor * motor;

// Controlling motor acceleration
void IRAM_ATTR interruptMotorSecondHand(void* arg) {
	if(motor->getStatus() == Motor::RUNNING && motor->incrementAngularVelocity() > 800) {
		esp_timer_stop(motor->secondHandTimer);
		motor->secondHandTimer = nullptr;
	}
	if(motor->getStatus() == Motor::RUNNING_WITH_BREAK && motor->decrementAngularVelocity() == 0) {
		esp_timer_stop(motor->secondHandTimer);
		motor->secondHandTimer = nullptr;
	}
}

void setup() {
	Serial2.begin(115200, SERIAL_8N1, 5, 14);
	pinMode(25, OUTPUT);
	pinMode(26, OUTPUT);
	pinMode(27, OUTPUT);
	Serial.begin(115200);

	motor = Motor::getInstance();

	Display * display = new Display("hmi");
	display->start();
}

TickType_t xDelay = 1 / portTICK_PERIOD_MS;

void loop() {
	vTaskDelay(xDelay);
}
