#include "Display.hpp"

#include "stone.h"

Display::Display(const char * name) : Thread(name) {
}

void Display::run(void* data) {
	extern unsigned char receive_over_flage;

	unsigned long timer = xTaskGetTickCount();
	TickType_t xDelay = 1 / portTICK_PERIOD_MS;
	Serial2.flush();
	Serial.print("Display::run");

	while(true) {
		vTaskDelay(xDelay);

		if(this->control->getDisplayStatus() == Control::RECEIVING) {
			serial_receive();
			if(receive_over_flage == 1) {
				Serial.println("receive_over_flage : 1");
				this->parseIncome(nullptr);
				_stone_recive_free(NULL); //Manual release the allocated space
				// Resetting
				receive_over_flage = 0;
			}
			continue;
		}

		if(this->control->getDisplayStatus() == Control::SENDING) {
			if(this->control->messagesQueue.count()) {
				// Flag for doing pauses between sending
				int pauseFlag = 0;
				Serial2.flush();
				while(this->control->messagesQueue.count()) {
					if(++pauseFlag & 1) { // Each two times
						vTaskDelay(15 / portTICK_PERIOD_MS); // Lightweight pause
					}
					String tempItem = this->control->messagesQueue.pop();
					Serial2.print(tempItem);
					Serial.print("Out cmd: ");
					Serial.println(tempItem);
				}
				pauseFlag = 0;
				Serial2.print("\n");
				Serial2.flush();
			}

			this->control->setDisplayReceiving();
		}
	}
}

void Display::parseIncome(void * data) {
	extern recive_group STONER;
	extern unsigned char STONE_RX_BUF[RX_LEN];

	String widgetName((char*)STONER.widget);

	Serial.print("cmd: "); Serial.println(STONER.cmd);
	Serial.print("len: "); Serial.println(STONER.len);
	Serial.print("value:"); Serial.println(STONER.value);
	Serial.print("float_value: "); Serial.println(STONER.float_value);
	Serial.print("long_value: "); Serial.println(STONER.long_value);
	Serial.print("Widget: "); Serial.println(widgetName);
	Serial.print("data: "); Serial.println(STONER.data);
	Serial.print("name:");
	for(int wds = 0; wds < STONER.len; wds++) {
		Serial.print(widgetName[wds]);
	}
	Serial.print("\ntext: "); Serial.println(STONER.text);
	Serial.print("END\n\n");

	switch(STONER.cmd) {
		case 0: // Display has booted
			return;

		case 4097:
		case 4098: // Buttons
			if(widgetName.startsWith("btnStart")) {
				return;
			}

			if(widgetName.startsWith("btnStop")) {
				return;
			}

			if(widgetName.startsWith("btnHome")) {
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"back_win\",\"type\":\"window\"}>ET"));
				this->control->setDisplaySending();
				return;
			}

			if(widgetName.startsWith("btnConfig")) {
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"open_win\",\"type\":\"window\",\"widget\":\"configPage\"}>ET"));
				this->control->setDisplaySending();
				return;
			}

			if(widgetName.startsWith("btnHistory")) {
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"open_win\",\"type\":\"window\",\"widget\":\"historyPage\"}>ET"));
				this->control->setDisplaySending();
				return;
			}

			if(widgetName.startsWith("btnLogo")) {
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"open_win\",\"type\":\"window\",\"widget\":\"sslPage\"}>ET"));
				this->control->setDisplaySending();
				return;
			}

			break;
	}
}
