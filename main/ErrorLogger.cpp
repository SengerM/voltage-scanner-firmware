#include "ErrorLogger.h"

Error::Error() {
	this-> severity = NO_ERROR;
	this-> msg_length = 0;
}

Error::Error(const Error & err) {
	this->severity = err.severity;
	unsigned int i=0;
	while (err.msg[i] != '\0') {
		this->msg[i] = err.msg[i];
		if (++i == MAX_ERROR_MSG_LENGTH) break;
	}
	this->msg_length = i;
}

Error::Error(error_severity_t _severity, char * _msg) {
	this->severity = _severity;
	unsigned int i=0;
	while (_msg[i] != '\0') {
		this->msg[i] = _msg[i];
		if (++i == MAX_ERROR_MSG_LENGTH) break;
	}
	this->msg_length = i;
}

void Error::report_serial() {
	switch (this->severity) {
		case NO_ERROR: return;
		case WARNING: Serial.print(F("WARNING: ")); break;
		case ERROR: Serial.print(F("ERROR: ")); break;
		case FATAL: Serial.print(F("FATAL: ")); break;
		default: return;
	}
	for (int i=0; i<this->msg_length; i++)
		Serial.print(this->msg[i]);
	Serial.print(LINE_TERMINATION);
}

ErrorLogger::ErrorLogger(void) {
	this->error_led_pin = -1;
	this->n_errors = 0;
}

ErrorLogger::ErrorLogger(int error_led_pin) {
	this->error_led_pin = error_led_pin;
	pinMode(error_led_pin, OUTPUT);
	this->n_errors = 0;
}

void ErrorLogger::new_error(const Error & error) {
	if (this->n_errors < MAX_ERROR_LOGS) {
		this->errors[this->n_errors] = error;
		this->n_errors += 1;
	} else {
		Serial.print(F(ERROR_LOGGER_MSG_HEADDER));
		Serial.print(F("Error logger is full, cannot log the following error: "));
		error.report_serial();
	}
	if (this->error_led_pin >= 0) digitalWrite(error_led_pin, HIGH);
}

void ErrorLogger::report_first_error() {
	if (this->n_errors > 0) {
		this->errors[0].report_serial();
		for (int i=1; i < this->n_errors; i++)
			this->errors[i-1] = this->errors[i];
		this->n_errors -= 1;
	} else {
		Serial.print(F(ERROR_LOGGER_MSG_HEADDER));
		Serial.print(F("Empty, no errors"));
		Serial.print(LINE_TERMINATION);
	}
	if (this->n_errors == 0)
		if (this->error_led_pin >= 0) digitalWrite(error_led_pin, LOW);
}

void ErrorLogger::report_all_errors() {
	while (this->n_errors > 0)
		this->report_first_error();
}
