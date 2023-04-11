#ifndef MICROUI_RENDER_H
#define MICROUI_RENDER_H

#include "microui.h"

void MicroUIRender_init();
void MicroUIRender_shutdown();
void MicroUIRender_draw(int width, int height);
mu_Context* MicroUIRender_getContext();
void MicroUIRender_style_window(mu_Context *ctx);

#endif // !MICROUI_RENDER_H
