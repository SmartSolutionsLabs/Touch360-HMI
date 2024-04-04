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
}

void Control::setDisplayReceiving() {
	this->displayStatus = Control::RECEIVING;
}

Control::DisplayStatus Control::getDisplayStatus() const {
	return this->displayStatus;
}
