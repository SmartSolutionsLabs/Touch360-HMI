#include "Display.hpp"
#include "Control.hpp"
#include "Motor.hpp"
#include "Status.hpp"
#include "Glooger.hpp"

Motor * motor;
Application * control;

// Controlling motor acceleration
void IRAM_ATTR interruptMotorSecondHand(void* arg) {
	if(motor->getStatus() == Status::RUNNING) {
		motor->incrementAngularVelocity();
	}
	if(motor->getStatus() == Status::RUNNING_WITH_BREAK && motor->decrementAngularVelocity() == 0) {
		esp_timer_stop(motor->secondHandTimer);
		motor->secondHandTimer = nullptr;
	}
}

void setup() {
	Serial.begin(115200);

	control = new Control;
	control->initializeModulesPointerArray();
	control->beginSerialPort(Serial2, SERIAL_8N1, 5, 14);

	Glooger * glooger = new Glooger("ggl");
	glooger->start();
}

TickType_t xDelay = 1 / portTICK_PERIOD_MS;

void loop() {
	vTaskDelay(xDelay);
}
