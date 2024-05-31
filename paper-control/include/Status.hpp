#ifndef STATUS_INC
#define STATUS_INC

/**
 * Status of the motor.
 */
enum Status {
	OFF = 0,
	TEST = 1,
	RUNNING = 2,
	RUNNING_WITH_BREAK = 3,
	PAUSED = 4,
	PAUSED_BY_ERROR = 5,
	HALTED = 6,
	FINISHED = 7,

	// Only for logs
	ON =  100,
	RUNNING_AFTER_PAUSED = 101
};

#define TEXT_STATUS_OFF "Apagado"
#define TEXT_STATUS_TEST "Test"
#define TEXT_STATUS_RUNNING "Corriendo"
#define TEXT_STATUS_RUNNING_WITH_BREAK "Frenando"
#define TEXT_STATUS_PAUSED "Pausado"
#define TEXT_STATUS_PAUSED_BY_ERROR "Pausado por error"
#define TEXT_STATUS_HALTED "Detenido por emergencia"
#define TEXT_STATUS_FINISHED "Acabado bien"

#define TEXT_STATUS_ON "Conectado"
#define TEXT_STATUS_RUNNING_AFTER_PAUSED "Reanudado"

#endif
