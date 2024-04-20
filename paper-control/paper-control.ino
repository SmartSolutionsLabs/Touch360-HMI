#include "Display.hpp"

// When laser detects paper cut
void IRAM_ATTR interruptCutterHandler() {
}

void setup() {
	Serial2.begin(115200);
	pinMode(25, OUTPUT);
	pinMode(26, OUTPUT);
	pinMode(27, OUTPUT);
	Serial.begin(115200);

	attachInterrupt(digitalPinToInterrupt(35), interruptCutterHandler, FALLING);

	Display * display = new Display("hmi");
	display->start();
}

TickType_t xDelay = 1 / portTICK_PERIOD_MS;

void loop() {
	vTaskDelay(xDelay);
}
