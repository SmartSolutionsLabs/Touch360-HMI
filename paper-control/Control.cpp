#include "Control.hpp"

Control * Control::control = nullptr;

Control * Control::getInstance() {
	if(control == nullptr) {
		control = new Control();
	}

	return control;
}

Control::Control() : displayStatus(RECEIVING), messagesQueue(25) {
	unsigned int i = (sizeof(this->rolls) / sizeof(*this->rolls));
	char strRoll[5];
	while(--i) {
		sprintf(strRoll, "roll%d", i);
		this->rolls[i].preferences.begin(strRoll, false);
		this->rolls[i].maxSpinsQuantity = this->rolls[i].preferences.getUInt("spins", 0);
		this->rolls[i].name = this->rolls[i].preferences.getString("name", strRoll);

		if(i == 0) {
			break;
		}
	}
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

void Control::setRollQuantity(unsigned int typeIndex, unsigned int position, unsigned int newValue) {
	unsigned int quantity = this->rolls[typeIndex].maxSpinsQuantity;

	// Separate digits
	unsigned int digits[4];
	digits[0] = quantity % 10;
	digits[1] = (quantity / 10) % 10;
	digits[2] = (quantity / 100) % 10;
	digits[3] = (quantity / 1000) % 10;

	digits[position] = newValue;
	newValue = 0; // Clean to reusing below
	position = 1; // Clean to reusing below

	// thousandsDigit * 1000 + hundredsDigit * 100 + tensDigit * 10 + unitsDigit
	for(unsigned int i = 0; i < 4; ++i) {
		newValue += digits[i] * position;
		position *= 10;
	}

	this->rolls[typeIndex].maxSpinsQuantity = newValue;
	this->rolls[typeIndex].preferences.putUInt("spins", newValue);
}

void Control::setRollName(unsigned int typeIndex, const char * name) {
	this->rolls[typeIndex].name = String(name);
	this->rolls[typeIndex].preferences.putString("name", this->rolls[typeIndex].name);
}
