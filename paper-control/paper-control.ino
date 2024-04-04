#include "Display.hpp"

void setup() {
	Serial2.begin(115200);
	pinMode(25, OUTPUT);
	pinMode(26, OUTPUT);
	pinMode(27, OUTPUT);
	Serial.begin(115200);

	Display * display = new Display("hmi");
	display->start();
}

TickType_t xDelay = 1 / portTICK_PERIOD_MS;

void loop() {
	vTaskDelay(xDelay);
}
