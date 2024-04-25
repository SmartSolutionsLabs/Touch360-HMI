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

Motor::Motor(const char * name) : Thread(name), maxSpinsQuantity(0), currentSpinsQuantity(0) {
}

void Motor::setMaxSpinsQuantity(unsigned int maxSpinsQuantity) {
	this->maxSpinsQuantity = maxSpinsQuantity;
}

void Motor::resetCurrentSpinsQuantity() {
	this->currentSpinsQuantity = 0;
}

unsigned int Motor::getAngularVelocity() const {
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

void Motor::halt() {
	this->status = Motor::HALTED;
}

void Motor::toggleStatus() {
	if(this->status == Motor::RUNNING) {
		this->status = Motor::PAUSED;
		return;
	}

	this->status = Motor::RUNNING;
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
}

void Motor::parseIncome(void * data) {
}
