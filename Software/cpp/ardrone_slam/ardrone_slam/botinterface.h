#pragma once

class botinterface
{
public:
	virtual void init(void) = 0; // required
	virtual void control_update(void *control) = 0; // required
	virtual void socket_callback(int id, char *message, int bytes) {} // not required

	/* aereal */
	virtual void take_off() {} // not required
	virtual void land() {} // not required
	virtual void recover(bool send) {} // not required
};