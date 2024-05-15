#include "Watcher.hpp"
#include "Motor.hpp"

#include "stone.h"

Watcher::Watcher(const char * name) : Thread(name) {
}

void Watcher::run(void* data) {
	Serial.print("Watcher::run");

	TickType_t xDelay = 200 / portTICK_PERIOD_MS;

	Motor * motor = Motor::getInstance();

	while(true) {
		vTaskDelay(xDelay);

		if(this->control->view != Control::HOME) {
			continue;
		}

		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsCurrent\",\"text\":\"" + String(motor->getCurrentSpinsQuantity()) + String("\"}>ET")));
		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":" + String( ceil((1.0f * motor->getCurrentSpinsQuantity()) / motor->getMaxSpinsQuantity() * 100) ) + "}>ET"));
		this->control->setDisplaySending();
	}
}

void Watcher::parseIncome(void * data) {
}
