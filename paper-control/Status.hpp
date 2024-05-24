#ifndef STATUS_INC
#define STATUS_INC

/**
 * Status of the motor.
 */
enum Status {
	OFF = 0,
	TEST,
	RUNNING,
	RUNNING_WITH_BREAK,
	PAUSED,
	PAUSED_BY_ERROR,
	HALTED,
	FINISHED
};

#endif
