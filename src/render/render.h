#ifndef RENDER_H
#define RENDER_H

//! Interface to renderer
class IRender {
public:
	virtual ~IRender() {}

	virtual void init() = 0;
	virtual void shutdown() = 0;
};

IRender* createRender();

#endif