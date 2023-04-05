#include "engine/engine.h"
#include "render/render.h"

class Main {
public:
	Main() :
		m_engine(nullptr)
	,	m_render(nullptr)
	{
	}

	int init(int argc, char* argv[]);
	void shutdown();

private:
	Engine* m_engine;
	IRender* m_render;
};

static Main s_main;

int Main::init(int argc, char* argv[]) {
	m_engine = new Engine();
	m_engine->init();

	m_render = createRender();
	m_render->init();

	return 0;
}

void Main::shutdown() {
	m_render->shutdown();
	delete m_render;

	m_engine->shutdown();
	delete m_engine;
}

int main(int argc, char* argv[]) {

	int retval = s_main.init(argc, argv);
	if (!retval)
		return retval;

	s_main.shutdown();

	return 0;
}

