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

		if(motor->getPaperUpStatus() == Commodity::CUT) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgPaperUpX\",\"visible\":true}>ET"));
			this->control->setDisplaySending();
		}
		else {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgPaperUpX\",\"visible\":false}>ET"));
			this->control->setDisplaySending();
		}

		if(motor->getPaperDownStatus() == Commodity::CUT) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgPaperDownX\",\"visible\":true}>ET"));
			this->control->setDisplaySending();
		}
		else {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgPaperDownX\",\"visible\":false}>ET"));
			this->control->setDisplaySending();
		}

		if(motor->getStatus() == Motor::FINISHED) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStop\",\"text\":\"Nuevo\"}>ET"));
		}
		else {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStop\",\"text\":\"Parar\"}>ET"));
		}

		if(motor->getStatus() == Motor::HALTED) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":0}>ET"));
		}

		if(motor->getStatus() != Motor::RUNNING && motor->getStatus() != Motor::RUNNING_WITH_BREAK) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgStop\",\"visible\":true}>ET"));
			this->control->setDisplaySending();
			continue;
		}

		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgStop\",\"visible\":false}>ET"));
		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStop\",\"text\":\"Parar\"}>ET"));

		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsCurrent\",\"text\":\"" + String(motor->getCurrentSpinsQuantity()) + String("\"}>ET")));
		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":" + String( ceil((1.0f * motor->getCurrentSpinsQuantity()) / motor->getMaxSpinsQuantity() * 100) ) + "}>ET"));
		this->control->setDisplaySending();
	}
}

void Watcher::parseIncome(void * data) {
}
