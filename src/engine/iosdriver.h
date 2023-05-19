#ifndef IOSDRIVER_H
#define IOSDRIVER_H

class IOsDriver
{
public:
	static IOsDriver* getInstance();

public:
	virtual ~IOsDriver() {}

	virtual void init() = 0;
	virtual void shutdown() = 0;

	virtual const char* getCurrentDirectory() = 0;
};

#endif // !IOSDRIVER_H
