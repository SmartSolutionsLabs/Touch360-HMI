#include "Motor.hpp"

#include <Adafruit_PCF8574.h>
#include <Adafruit_PWMServoDriver.h>

Motor * Motor::motor = nullptr;

Motor * Motor::getInstance() {
	if(motor == nullptr) {
		motor = new Motor("motor");
	}

	return motor;
}

Motor::Motor() : Thread("mtr", 1) {
}

Motor::Motor(const char * name) : Thread(name), maxSpinsQuantity(0), currentSpinsQuantity(0), angularVelocity(0), paperDownStatus(Commodity::MISSING), paperUpStatus(Commodity::MISSING) {
	this->maxAngularVelocity = this->control->getMaxVelocity();

	// This motor will run forever
	this->start();
}

void Motor::setMaxSpinsQuantity(unsigned int maxSpinsQuantity) {
	this->maxSpinsQuantity = maxSpinsQuantity;
}

void Motor::resetCurrentSpinsQuantity() {
	this->currentSpinsQuantity = 0;
}

int Motor::getAngularVelocity() const {
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

int Motor::incrementAngularVelocity() {
	if(this->angularVelocity > this->maxAngularVelocity) {
		this->angularVelocity = this->maxAngularVelocity;
		return this->angularVelocity;
	}

	return ++this->angularVelocity;
}

int Motor::decrementAngularVelocity() {
	if(this->angularVelocity < 0) {
		this->angularVelocity = 0;
	}
	else {
		this->angularVelocity--;
	}

	return this->angularVelocity;
}

void Motor::setMaxAngularVelocity(int maxAngularVelocity) {
	if(angularVelocity > MAX_MOTOR_VELOCITY) {
		this->angularVelocity = MAX_MOTOR_VELOCITY;
		return;
	}

	this->maxAngularVelocity = maxAngularVelocity;
}

int Motor::getMaxAngularVelocity() const {
	return this->maxAngularVelocity;
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
	if(this->status == Motor::RUNNING || this->status == Motor::RUNNING_WITH_BREAK) {
		this->status = Motor::PAUSED;
		this->angularVelocity = 0; // by the way
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
	esp_timer_start_periodic(this->secondHandTimer, 1000000 / 100); // Each fraction of second
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
	TickType_t xDelay = 1 / portTICK_PERIOD_MS; // Normal speed

	TickType_t xDelayForPrinting = 200 / portTICK_PERIOD_MS; // Printing speed
	TickType_t xMilestone = xTaskGetTickCount();

	Wire.begin(4, 16);

	Adafruit_PWMServoDriver motorControl(0x40, Wire);
	motorControl.begin();
	motorControl.setPin(0, 0);
	motorControl.setPin(1, 0);
	motorControl.setPin(2, 0);
	motorControl.setPin(3, 0);
	motorControl.setPin(4, 0);
	motorControl.setPin(5, 0);
	motorControl.setPin(6, 0);
	motorControl.setPin(7, 0); // Attached to this motor
	motorControl.setOscillatorFrequency(27000000);
	motorControl.setPWMFreq(1000);

	Adafruit_PCF8574 remoteControl; // laser for papers and button inputs

	if(!remoteControl.begin(0x24, &Wire)) {
		Serial.println("Couldn't find PCF8574 in 0x24");

		//Show warnings
	}
	else {
		// Setting
		remoteControl.pinMode(PIN_SPIN, INPUT_PULLUP); // spin
		remoteControl.pinMode(PIN_PAPER_DOWN, INPUT_PULLUP); // spin
		remoteControl.pinMode(PIN_PAPER_UP, INPUT_PULLUP); // spin
		remoteControl.pinMode(PIN_TEST , INPUT_PULLUP);

		remoteControl.pinMode(PIN_MOTOR, OUTPUT); // enable or disable this motor
		remoteControl.pinMode(PIN_ELECTROVALVE, OUTPUT); // electrovalves for pistons

		remoteControl.digitalWrite(PIN_MOTOR, HIGH);
		remoteControl.digitalWrite(PIN_ELECTROVALVE, LOW);
	}

	int angularVelocity = 0; // For comparing and change it if is needed

	bool previousMotorSpinRead = 0; //remoteControl.digitalRead(PIN_SPIN);
	bool currentMotorSpinRead = previousMotorSpinRead;

	Status previousMotorStatus = Motor::OFF;

	while(1) {
		vTaskDelay(xDelay);

		// EMERGENCY TO AVOID GO OUT OF RANGE !
		if(this->angularVelocity > MAX_MOTOR_VELOCITY) {
			this->angularVelocity = MAX_MOTOR_VELOCITY;
			angularVelocity = MAX_MOTOR_VELOCITY;
		}

		if(!remoteControl.digitalRead(PIN_TEST) && this->status != Motor::TEST){
			this->status = Motor::TEST;
			this->angularVelocity = 100;
			remoteControl.digitalWrite(PIN_MOTOR, LOW);
			motorControl.setPin(7, angularVelocity);
		}

		if(remoteControl.digitalRead(PIN_TEST) && this->status == Motor::TEST){
			this->status = Motor::OFF;
			this->angularVelocity = 0;
			remoteControl.digitalWrite(PIN_MOTOR, HIGH);
			motorControl.setPin(7, angularVelocity);
		}

		// Starting motor when was in another status
		if(previousMotorStatus != Motor::RUNNING && this->status == Motor::RUNNING) {
			previousMotorStatus == this->status;
			remoteControl.digitalWrite(PIN_MOTOR, LOW);
			remoteControl.digitalWrite(PIN_ELECTROVALVE, LOW);
		}

		if((this->status == Motor::PAUSED || this->status == Motor::HALTED) && (previousMotorStatus != Motor::PAUSED || previousMotorStatus != Motor::HALTED)) {
			previousMotorStatus == this->status;
			remoteControl.digitalWrite(PIN_MOTOR, HIGH);
			remoteControl.digitalWrite(PIN_ELECTROVALVE, HIGH);
			motorControl.setPin(7, 0);
		}

		currentMotorSpinRead = remoteControl.digitalRead(PIN_SPIN);

		if(currentMotorSpinRead != previousMotorSpinRead) {
			previousMotorSpinRead = currentMotorSpinRead;
			if(currentMotorSpinRead){
				Serial.print("step\t");
				this->incrementCurrentSpinsQuantity();
				Serial.println(this->currentSpinsQuantity);
			}
		}

		if(angularVelocity != 0 && this->status == Motor::HALTED) {
			angularVelocity = 0;

			motorControl.setPin(7, 0);
			Serial.print("Halt&setPWM 0");
		}

		// Translate the local velocity to this motor
		if(this->angularVelocity != angularVelocity) {
			motorControl.setPin(7, this->angularVelocity);
			angularVelocity = this->angularVelocity;
			Serial.print("angVel:");
			Serial.println(this->angularVelocity);
		}

		// Test up paper and change it
		if(!remoteControl.digitalRead(PIN_PAPER_UP)) {
			if(this->paperUpStatus != Commodity::PRESENT) {
				this->paperUpStatus = Commodity::PRESENT;
				Serial.print("Paper up present\n");
			}
		}
		else {
			if(this->paperUpStatus != Commodity::CUT) {
				this->paperUpStatus = Commodity::CUT;
				Serial.print("Paper up cut\n");
			}
		}

		// Test down paper and change it
		if(!remoteControl.digitalRead(PIN_PAPER_DOWN)) {
			if(this->paperDownStatus != Commodity::PRESENT) {
				this->paperDownStatus = Commodity::PRESENT;
				Serial.print("Paper down present\n");
			}
		}
		else {
			if(this->paperDownStatus != Commodity::CUT) {
				this->paperDownStatus = Commodity::CUT;
				Serial.print("Paper down cut\n");
			}
		}

		if((this->paperDownStatus == Commodity::CUT || this->paperUpStatus == Commodity::CUT) && (this->status == Motor::RUNNING || this->status == Motor::RUNNING_WITH_BREAK)) {
			remoteControl.digitalWrite(PIN_MOTOR, HIGH);
			this->halt();
		}

		// Only repaint when motor is working
		if(this->status != Motor::RUNNING && this->status != Motor::RUNNING_WITH_BREAK) {
			continue;
		}

		int delta = this->maxSpinsQuantity * 30 / 100;
		if(delta > 30) delta = 30;
		if((this->currentSpinsQuantity > (this->maxSpinsQuantity - delta )) && this->status == Motor::RUNNING) {
			this->status = Motor::RUNNING_WITH_BREAK;
		}

		if(this->currentSpinsQuantity >= this->maxSpinsQuantity) {
			remoteControl.digitalWrite(PIN_MOTOR, HIGH);
			this->halt();
			this->status = Motor::FINISHED; // Stopped gracefully
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_visible\",\"type\":\"widget\",\"widget\":\"imgStop\",\"visible\":true}>ET"));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_color\",\"type\":\"widget\",\"widget\":\"barProgress\",\"color_object\":\"fg_color\",\"color\":4278255104}>ET"));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"progress_bar\",\"widget\":\"barProgress\",\"value\":100}>ET"));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStart\",\"text\":\"Iniciar\"}>ET"));
			//~ this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"btnStop\",\"text\":\"Nuevo\"}>ET"));
			this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsCurrent\",\"text\":\"" + String(this->getCurrentSpinsQuantity()) + String("\"}>ET")));
			this->control->setDisplaySending();
		}
	}
}

void Motor::parseIncome(void * data) {
}
