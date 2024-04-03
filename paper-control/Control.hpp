#ifndef CONTROL_INC
#define CONTROL_INC

class Control {
	protected:
		static Control * control;
		Control();

	public:
		// For singleton
		static Control * getInstance();
		Control(Control &other) = delete;
		void operator=(const Control &) = delete;
};

#endif
