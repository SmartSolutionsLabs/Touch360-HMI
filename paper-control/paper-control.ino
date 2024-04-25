#include "Display.hpp"
#include "Control.hpp"
#include "Motor.hpp"

Motor * motor;

// When laser detects paper cut
void IRAM_ATTR interruptCutterHandler() {
	motor->incrementCurrentSpinsQuantity();
}

void setup() {
	Serial2.begin(115200);
	pinMode(25, OUTPUT);
	pinMode(26, OUTPUT);
	pinMode(27, OUTPUT);
	Serial.begin(115200);

	motor = Motor::getInstance();

	attachInterrupt(digitalPinToInterrupt(35), interruptCutterHandler, FALLING);

	Display * display = new Display("hmi");
	display->start();
}

TickType_t xDelay = 120 / portTICK_PERIOD_MS;

void loop() {
	vTaskDelay(xDelay);

	// Only repaint when motor is working
	if(motor->getStatus() == Motor::RUNNING) {
		Control::getInstance()->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsCurrent\",\"text\":\"" + String(motor->getCurrentSpinsQuantity()) + String("\"}>ET")));
		Control::getInstance()->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":" + String( ceil((1.0f * motor->getCurrentSpinsQuantity()) / motor->getMaxSpinsQuantity() * 100) ) + "}>ET"));
		Control::getInstance()->setDisplaySending();
	}
}
