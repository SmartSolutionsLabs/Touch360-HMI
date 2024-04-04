#include "Control.hpp"

Control * Control::control = nullptr;

Control * Control::getInstance() {
	if(control == nullptr) {
		control = new Control();
	}

	return control;
}

Control::Control() : displayStatus(RECEIVING) {
}

void Control::setDisplaySending() {
	this->displayStatus = Control::SENDING;

	// For receiving always pins are high
	digitalWrite(25, HIGH);
	digitalWrite(26, HIGH);
	digitalWrite(27, HIGH);
}

void Control::setDisplayReceiving() {
	this->displayStatus = Control::RECEIVING;

	// For receiving always pins are low
	digitalWrite(25, LOW);
	digitalWrite(26, LOW);
	digitalWrite(27, LOW);
}

Control::DisplayStatus Control::getDisplayStatus() const {
	return this->displayStatus;
}
