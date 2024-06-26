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
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"open_win\",\"type\":\"window\",\"widget\":\"home_page\"}>ET"));
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

		case 4144: {
			if(widgetName.startsWith("typeSelected1")) {
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsTotal\",\"text\":\"" + String(this->control->getRollQuantity(0)) + String("\"}>ET")));
				this->control->setDisplaySending();
				return;
			}
			if(widgetName.startsWith("typeSelected2")) {
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsTotal\",\"text\":\"" + String(this->control->getRollQuantity(1)) + String("\"}>ET")));
				this->control->setDisplaySending();
				return;
			}
			if(widgetName.startsWith("typeSelected3")) {
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSpinsTotal\",\"text\":\"" + String(this->control->getRollQuantity(2)) + String("\"}>ET")));
				this->control->setDisplaySending();
				return;
			}
		}

		case 4208: // Edit
			if(widgetName.startsWith("edtPaper1")) {
				this->control->setRollName(0, STONER.text);
				return;
			}
			if(widgetName.startsWith("edtPaper2")) {
				this->control->setRollName(1, STONER.text);
				return;
			}
			if(widgetName.startsWith("edtPaper3")) {
				this->control->setRollName(2, STONER.text);
				return;
			}


		case 4225: // Spins
			if(widgetName.startsWith("spinner14")) {
				this->control->setRollQuantity(0, 3, STONER.long_value);
				return;
			}
			if(widgetName.startsWith("spinner13")) {
				this->control->setRollQuantity(0, 2, STONER.long_value);
				return;
			}
			if(widgetName.startsWith("spinner12")) {
				this->control->setRollQuantity(0, 1, STONER.long_value);
				return;
			}
			if(widgetName.startsWith("spinner11")) {
				this->control->setRollQuantity(0, 0, STONER.long_value);
				return;
			}

			if(widgetName.startsWith("spinner24")) {
				this->control->setRollQuantity(1, 3, STONER.long_value);
				return;
			}
			if(widgetName.startsWith("spinner23")) {
				this->control->setRollQuantity(1, 2, STONER.long_value);
				return;
			}
			if(widgetName.startsWith("spinner22")) {
				this->control->setRollQuantity(1, 1, STONER.long_value);
				return;
			}
			if(widgetName.startsWith("spinner21")) {
				this->control->setRollQuantity(1, 0, STONER.long_value);
				return;
			}

			if(widgetName.startsWith("spinner34")) {
				this->control->setRollQuantity(2, 3, STONER.long_value);
				return;
			}
			if(widgetName.startsWith("spinner33")) {
				this->control->setRollQuantity(2, 2, STONER.long_value);
				return;
			}
			if(widgetName.startsWith("spinner32")) {
				this->control->setRollQuantity(2, 1, STONER.long_value);
				return;
			}
			if(widgetName.startsWith("spinner31")) {
				this->control->setRollQuantity(2, 0, STONER.long_value);
				return;
			}

			break;

		case 8199:
			if(STONER.len == 9) { //home page
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"lblSelected1_3\",\"text\":[\"" + this->control->getRollName(0) + "\",\"" + this->control->getRollName(1) + "\",\"" + this->control->getRollName(2) + "\"]}>ET"));
				this->control->setDisplaySending();
				return;
			}

			if(STONER.len == 10) { //configuration page
				unsigned int spinsQuantity = 0;
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_text\",\"type\":\"edit\",\"widget\":\"edtPaper1_3\",\"text\":[\"" + this->control->getRollName(0) + "\",\"" + this->control->getRollName(1) + "\",\"" + this->control->getRollName(2) + "\"]}>ET"));

				spinsQuantity = this->control->getRollQuantity(0);
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"text_selector\",\"widget\":\"spinner11_14\",\"value\":[" + String(spinsQuantity % 10) + String(",") + ((spinsQuantity / 10) % 10) + String(",") + ((spinsQuantity / 100) % 10) + String(",") + ((spinsQuantity / 1000) % 10) + String("]}>ET")));
				spinsQuantity = this->control->getRollQuantity(1);
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"text_selector\",\"widget\":\"spinner21_24\",\"value\":[" + String(spinsQuantity % 10) + String(",") + ((spinsQuantity / 10) % 10) + String(",") + ((spinsQuantity / 100) % 10) + String(",") + ((spinsQuantity / 1000) % 10) + String("]}>ET")));
				spinsQuantity = this->control->getRollQuantity(2);
				this->control->messagesQueue.push(String("ST<{\"cmd_code\":\"set_value\",\"type\":\"text_selector\",\"widget\":\"spinner31_34\",\"value\":[" + String(spinsQuantity % 10) + String(",") + ((spinsQuantity / 10) % 10) + String(",") + ((spinsQuantity / 100) % 10) + String(",") + ((spinsQuantity / 1000) % 10) + String("]}>ET")));

				this->control->setDisplaySending();
				return;
			}

			break;
	}
}
