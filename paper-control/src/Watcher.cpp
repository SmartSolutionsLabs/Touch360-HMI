#include "Watcher.hpp"
#include "Motor.hpp"
#include "Status.hpp"

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

		// Debugger on display
		String info;
		switch(motor->getStatus()) {
			case Status::OFF:
				info = "off";
				break;
			case Status::TEST:
				info = "testing. Velocity " + String(motor->getAngularVelocity());
				break;
			case Status::PAUSED:
				info = "paused";
				break;
			case Status::PAUSED_BY_ERROR:
				info = "paused by error";
				break;
			case Status::RUNNING:
				info = "running. Velocity " + String(motor->getAngularVelocity());
				break;
			case Status::RUNNING_WITH_BREAK:
				info = "runnning with break. Velocity " + String(motor->getAngularVelocity());
				break;
			case Status::HALTED:
				info = "halted";
				break;
			case Status::FINISHED:
				info = "finished";
				break;
		}
		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lbDebug\",\"text\":\"Motor " + info + String(".\"}>ET")));
		this->control->setDisplaySending();

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

		if(motor->getStatus() == Status::FINISHED) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStop\",\"text\":\"Nuevo\"}>ET"));
		}
		else {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStop\",\"text\":\"Parar\"}>ET"));
		}

		if(motor->getStatus() == Status::HALTED) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":0}>ET"));
		}

		if(Motor::getInstance()->getStatus() == Status::HALTED || Motor::getInstance()->getStatus() == Status::FINISHED) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_color\",\"type\":\"widget\",\"widget\":\"barProgress\",\"color_object\":\"fg_color\",\"color\":4278190334}>ET"));
		}

		if(motor->getStatus() == Status::PAUSED || motor->getStatus() == Status::PAUSED_BY_ERROR) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStart\",\"text\":\"Seguir\"}>ET"));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsCurrent\",\"text\":\"" + String(motor->getCurrentSpinsQuantity()) + String("\"}>ET")));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":" + String( ceil((1.0f * motor->getCurrentSpinsQuantity()) / motor->getMaxSpinsQuantity() * 100) ) + "}>ET"));
		}

		if(motor->getStatus() != Status::RUNNING && motor->getStatus() != Status::RUNNING_WITH_BREAK) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgStop\",\"visible\":true}>ET"));
			this->control->setDisplaySending();
			continue;
		}

		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgStop\",\"visible\":false}>ET"));
		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStop\",\"text\":\"Parar\"}>ET"));
		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStart\",\"text\":\"Pausar\"}>ET"));

		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsCurrent\",\"text\":\"" + String(motor->getCurrentSpinsQuantity()) + String("\"}>ET")));
		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":" + String( ceil((1.0f * motor->getCurrentSpinsQuantity()) / motor->getMaxSpinsQuantity() * 100) ) + "}>ET"));
		this->control->setDisplaySending();
	}
}

void Watcher::parseIncome(void * data) {
}
