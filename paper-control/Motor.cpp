#include "Motor.hpp"

#include <Adafruit_PCF8574.h>

Motor * Motor::motor = nullptr;

Motor * Motor::getInstance() {
	if(motor == nullptr) {
		motor = new Motor("motor");
	}

	return motor;
}

Motor::Motor() : Thread("mtr") {
}

Motor::Motor(const char * name) : Thread(name), maxSpinsQuantity(0), currentSpinsQuantity(0), angularVelocity(0), paperDownStatus(Commodity::MISSING), paperUpStatus(Commodity::MISSING) {
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

	Wire.begin(4, 16);
	Adafruit_PCF8574 remoteControl; // laser for papers and button inputs

	if(!remoteControl.begin(0x22, &Wire)) {
		Serial.println("Couldn't find PCF8574 in 0x22");

		//Show warnings
	}
	else {
		// Setting
		remoteControl.pinMode(0, INPUT_PULLUP); // paper up
		remoteControl.pinMode(1, INPUT_PULLUP); // paper down
	}

	while(1) {
		vTaskDelay(xDelay);

		// Test up paper and change it
		if(!remoteControl.digitalRead(0)) {
			if(this->paperUpStatus != Commodity::PRESENT) {
				this->paperUpStatus = Commodity::PRESENT;
				Serial.print("Paper up present\n");
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgPaperUpX\",\"visible\":false}>ET"));
			}
		}
		else {
			if(this->paperUpStatus != Commodity::CUT) {
				this->paperUpStatus = Commodity::CUT;
				Serial.print("Paper up cut\n");
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgPaperUpX\",\"visible\":true}>ET"));
			}
		}

		// Test down paper and change it
		if(!remoteControl.digitalRead(1)) {
			if(this->paperDownStatus != Commodity::PRESENT) {
				this->paperDownStatus = Commodity::PRESENT;
				Serial.print("Paper down present\n");
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgPaperDownX\",\"visible\":false}>ET"));
			}
		}
		else {
			if(this->paperDownStatus != Commodity::CUT) {
				this->paperDownStatus = Commodity::CUT;
				Serial.print("Paper down cut\n");
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgPaperDownX\",\"visible\":true}>ET"));
			}
		}

		if(this->paperDownStatus == Commodity::CUT || this->paperUpStatus == Commodity::CUT) {
			this->halt();
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgStop\",\"visible\":true}>ET"));
		}

		this->control->setDisplaySending();

		// Only repaint when motor is working
		if(this->status != Motor::RUNNING) {
			continue;
		}

		if(this->currentSpinsQuantity >= this->maxSpinsQuantity) {
			this->halt();
			this->status = Motor::FINISHED; // Stopped gracefully
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgStop\",\"visible\":true}>ET"));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_color\",\"type\":\"widget\",\"widget\":\"barProgress\",\"color_object\":\"fg_color\",\"color\":4278255104}>ET"));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_enable\",\"type\":\"widget\",\"widget\":\"btnStop\",\"enable\":false}>ET"));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":100}>ET"));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStart\",\"text\":\"Iniciar\"}>ET"));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsCurrent\",\"text\":\"" + String(this->getCurrentSpinsQuantity()) + String("\"}>ET")));
			this->control->setDisplaySending();
		}

		if(this->control->view != Control::HOME) {
			continue;
		}

		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsCurrent\",\"text\":\"" + String(this->getCurrentSpinsQuantity()) + String("\"}>ET")));
		this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":" + String( ceil((1.0f * this->getCurrentSpinsQuantity()) / this->getMaxSpinsQuantity() * 100) ) + "}>ET"));
		this->control->setDisplaySending();
	}
}

void Motor::parseIncome(void * data) {
}
