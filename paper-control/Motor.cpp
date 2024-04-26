#include "Motor.hpp"

Motor * Motor::motor = nullptr;

Motor * Motor::getInstance() {
	if(motor == nullptr) {
		motor = new Motor("motor");
	}

	return motor;
}

Motor::Motor() : Thread("mtr") {
}

Motor::Motor(const char * name) : Thread(name), maxSpinsQuantity(0), currentSpinsQuantity(0), angularVelocity(0) {
	// This motor will run forever
	this->start();
}

void Motor::setMaxSpinsQuantity(unsigned int maxSpinsQuantity) {
	this->maxSpinsQuantity = maxSpinsQuantity;
}

void Motor::resetCurrentSpinsQuantity() {
	this->currentSpinsQuantity = 0;
}

unsigned int Motor::getAngularVelocity() const {
	return this->angularVelocity;
}

unsigned int Motor::getMaxSpinsQuantity() const {
	return this->maxSpinsQuantity;
}

unsigned int Motor::getCurrentSpinsQuantity() const {
	return this->currentSpinsQuantity;
}

void Motor::incrementCurrentSpinsQuantity() {
	if(this->currentSpinsQuantity < this->maxSpinsQuantity) {
		++this->currentSpinsQuantity;
	}
}

unsigned int Motor::incrementAngularVelocity() {
	return ++this->angularVelocity;
}

void Motor::halt() {
	this->status = Motor::HALTED;

	this->angularVelocity = 0;

	if(this->secondHandTimer != nullptr) {
		esp_timer_stop(this->secondHandTimer);
		this->secondHandTimer = nullptr;
	}
}

void Motor::toggleStatus() {
	if(this->status == Motor::RUNNING) {
		this->status = Motor::PAUSED;
		return;
	}

	this->status = Motor::RUNNING;

	this->angularVelocity = 0;

	const esp_timer_create_args_t periodic_timer_args = {
			.callback = &interruptMotorSecondHand,
			.arg = NULL,
			.dispatch_method = ESP_TIMER_TASK,
			.name = "periodic_timer"
	};
	esp_timer_create(&periodic_timer_args, &this->secondHandTimer);
	esp_timer_start_periodic(this->secondHandTimer, 1000000); // Each 1 second
}

Motor::Status Motor::getStatus() const {
	return this->status;
}

Commodity Motor::getPaperUpStatus() const {
	return this->paperUpStatus;
}

Commodity Motor::getPaperDownStatus() const {
	return this->paperDownStatus;
}

void Motor::run(void* data) {
	TickType_t xDelay = 200 / portTICK_PERIOD_MS;

	while(1) {
		vTaskDelay(xDelay);

		// Only repaint when motor is working
		if(this->control->view == Control::HOME && motor->getStatus() == Motor::RUNNING) {
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsCurrent\",\"text\":\"" + String(this->getCurrentSpinsQuantity()) + String("\"}>ET")));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":" + String( ceil((1.0f * this->getCurrentSpinsQuantity()) / this->getMaxSpinsQuantity() * 100) ) + "}>ET"));
			this->control->setDisplaySending();
		}
	}
}

void Motor::parseIncome(void * data) {
}
