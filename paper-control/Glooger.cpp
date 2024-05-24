#include "Glooger.hpp"
#include "Status.hpp"
#include "private.hpp"

#include <ESP_Google_Sheet_Client.h>

Glooger::Glooger(const char * name) : Thread(name) {
}

void Glooger::run(void* data) {
	Serial.print("Glooger::run");

	const char privateKey[] PROGMEM = PRIVATE_GOOGLE_KEY;
	const char spreadsheetId[] = PRIVATE_GOOGLE_SPREADSHEET_ID;

	// Set the callback for Google API access token generation status (for debug only)
	//~ GSheet.setTokenCallback(tokenStatusCallback);

	// Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
	GSheet.setPrerefreshSeconds(10 * 60);

	// Begin the access token generation for Google API authentication
	GSheet.begin(PRIVATE_GOOGLE_CLIENT_EMAIL, PRIVATE_GOOGLE_PROJECT_ID, privateKey);

	TickType_t xDelay = 60 * 1000 / portTICK_PERIOD_MS;

	while(true) {
		vTaskDelay(xDelay);

		// Repeatedly for authentication checking and processing
		if(!GSheet.ready()) {
			continue;
		}

		// Only for testing
		this->control->addGloog(0, Control::LOG, Status::OFF, 41233);

		if(this->control->gloogerQueue.count() == 0) {
			continue;
		}

		FirebaseJson response;
		FirebaseJson valueRangeLog;
		FirebaseJson valueRangeStock;

		valueRangeLog.add("majorDimension", "COLUMNS");
		valueRangeStock.add("majorDimension", "COLUMNS");

		int logIndex = -1;
		int stockIndex = -1;
		// For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/append
		while(this->control->gloogerQueue.count()) {
			Gloog gloog = this->control->gloogerQueue.pop();

			switch(gloog.type) {
				case Control::LOG:
					++logIndex;
					valueRangeLog.set("values/[0]/[0]", gloog.unixtime);
					valueRangeLog.set("values/[0]/[1]", 1);
					break;
				case Control::STOCK:
					++stockIndex;
					valueRangeStock.set("values/[0]/[0]", gloog.unixtime);
					valueRangeStock.set("values/[0]/[1]", 2);
					valueRangeStock.set("values/[0]/[2]", gloog.data);
					break;
			}
		}

		// Append values to the spreadsheet
		if(logIndex >= 0) {
			bool success = GSheet.values.append(&response, spreadsheetId, "Log!A2", &valueRangeLog);
			if(success) {
				response.toString(Serial, true);
				valueRangeLog.clear();
			}
			else {
				Serial.println(GSheet.errorReason());
			}
		}

		if(stockIndex >= 0) {
			bool success = GSheet.values.append(&response, spreadsheetId, "Stock!A2", &valueRangeStock);
			if(success) {
				response.toString(Serial, true);
				valueRangeStock.clear();
			}
			else {
				Serial.println(GSheet.errorReason());
			}
		}
	}
}

void Glooger::parseIncome(void * data) {
}
