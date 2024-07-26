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

	TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
	//~ TickType_t xDelay = 60 * 1000 / portTICK_PERIOD_MS;

	while(true) {
		vTaskDelay(xDelay);

		// Repeatedly for authentication checking and processing
		if(!GSheet.ready()) {
			continue;
		}

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

			// Create string of epoch with formula
			char epochWithFormula[32];
			sprintf(epochWithFormula, "=EPOCHTODATE(%d)", gloog.unixtime - 5 * 60 * 60);

			switch(gloog.type) {
				case GloogerEvent::LOG:
					++logIndex;
					valueRangeLog.set("values/[0]/[0]", epochWithFormula);
					valueRangeLog.set("values/[1]/[0]", epochWithFormula);
					valueRangeLog.set("values/[2]/[0]",
						gloog.status == Status::OFF ? TEXT_STATUS_OFF :
						gloog.status == Status::TEST ? TEXT_STATUS_TEST :
						gloog.status == Status::RUNNING ? TEXT_STATUS_RUNNING :
						gloog.status == Status::RUNNING_WITH_BREAK ? TEXT_STATUS_RUNNING_WITH_BREAK :
						gloog.status == Status::PAUSED ? TEXT_STATUS_PAUSED :
						gloog.status == Status::PAUSED_BY_ERROR ? TEXT_STATUS_PAUSED_BY_ERROR :
						gloog.status == Status::HALTED ? TEXT_STATUS_HALTED :
						gloog.status == Status::FINISHED ? TEXT_STATUS_FINISHED :
						gloog.status == Status::ON ? TEXT_STATUS_ON :
						gloog.status == Status::RUNNING_AFTER_PAUSED ? TEXT_STATUS_RUNNING_AFTER_PAUSED :
						"-"
					);
					break;
				case GloogerEvent::STOCK:
					++stockIndex;
					valueRangeStock.set("values/[0]/[0]", epochWithFormula);
					valueRangeStock.set("values/[1]/[0]", epochWithFormula);
					valueRangeStock.set("values/[2]/[0]",
						gloog.status == Status::OFF ? TEXT_STATUS_OFF :
						gloog.status == Status::TEST ? TEXT_STATUS_TEST :
						gloog.status == Status::RUNNING ? TEXT_STATUS_RUNNING :
						gloog.status == Status::RUNNING_WITH_BREAK ? TEXT_STATUS_RUNNING_WITH_BREAK :
						gloog.status == Status::PAUSED ? TEXT_STATUS_PAUSED :
						gloog.status == Status::PAUSED_BY_ERROR ? TEXT_STATUS_PAUSED_BY_ERROR :
						gloog.status == Status::HALTED ? TEXT_STATUS_HALTED :
						gloog.status == Status::FINISHED ? TEXT_STATUS_FINISHED :
						gloog.status == Status::ON ? TEXT_STATUS_ON :
						gloog.status == Status::RUNNING_AFTER_PAUSED ? TEXT_STATUS_RUNNING_AFTER_PAUSED :
						"-"
					);
					valueRangeStock.set("values/[3]/[0]", gloog.data);
					break;
			}

			// Cleaning formula
			memset(epochWithFormula, '\0', sizeof(epochWithFormula));
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
