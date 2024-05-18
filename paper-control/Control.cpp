#include "Control.hpp"
#include "Network.hpp"

Control * Control::control = nullptr;

Control * Control::getInstance() {
	if(control == nullptr) {
		control = new Control();
	}

	return control;
}

Control::Control() : displayStatus(RECEIVING), messagesQueue(25), view(HOME) {
	unsigned int rollIndex = (sizeof(this->rolls) / sizeof(*this->rolls));
	char strRoll[5];
	while(rollIndex--) {
		sprintf(strRoll, "roll%d", rollIndex);
		this->rolls[rollIndex].preferences.begin(strRoll, false);
		this->rolls[rollIndex].maxSpinsQuantity = this->rolls[rollIndex].preferences.getUInt("spins", 0);
		this->rolls[rollIndex].name = this->rolls[rollIndex].preferences.getString("name", strRoll);
	}

	this->preferences.begin("global", false);

	Network::SSID = this->preferences.getString("netSsid", "");
	Network::PASSWORD = this->preferences.getString("netPassword", "");

	Network::getInstance()->connect();
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

unsigned int Control::getRollQuantity(unsigned int typeIndex) const {
	return this->rolls[typeIndex].maxSpinsQuantity;
}

void Control::setRollName(unsigned int typeIndex, const char * name) {
	this->rolls[typeIndex].name = String(name);
	this->rolls[typeIndex].preferences.putString("name", this->rolls[typeIndex].name);
}

String Control::getRollName(unsigned int typeIndex) const {
	return this->rolls[typeIndex].name;
}

void Control::saveMaxVelocity(int maxVelocity) {
	this->preferences.putInt("maxVelocity", maxVelocity);
}

int Control::getMaxVelocity() {
	return this->preferences.getInt("maxVelocity", 0);
}

void Control::setNetworkPassword(String networkPassword) {
	Network::PASSWORD = networkPassword;

	this->preferences.putString("netSsid", networkPassword);
}

void Control::setNetworkSsid(String networkSsid) {
	Network::SSID = networkSsid;

	this->preferences.putString("netSsid", networkSsid);
}
