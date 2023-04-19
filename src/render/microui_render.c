#include "render/microui_render.h"

#include "microui.h"

#include "atlas.inl"

#define SOKOL_GLCORE33
#include "sokol_gfx.h"
#include "sokol_log.h"

#define SOKOL_GL_IMPL
#include "util/sokol_gl.h"

static mu_Context mu_ctx;
static sg_image atlas_img;
static sgl_pipeline pip;

static int r_get_text_width(const char* text, int len);
static int r_get_text_height(void);

/* callbacks */
static int text_width_cb(mu_Font font, const char* text, int len) {
	(void)font;
	if (len == -1) {
		len = (int)strlen(text);
	}
	return r_get_text_width(text, len);
}

static int text_height_cb(mu_Font font) {
	(void)font;
	return r_get_text_height();
}

void MicroUIRender_init() {
	sgl_desc_t sgl_desc;
	memset(&sgl_desc, 0, sizeof(sgl_desc));
	sgl_desc.logger.func = slog_func;
	sgl_setup(&sgl_desc);

	uint32_t rgba8_size = ATLAS_WIDTH * ATLAS_HEIGHT * 4;
	uint32_t* rgba8_pixels = (uint32_t*)malloc(rgba8_size);
	for (int y = 0; y < ATLAS_HEIGHT; y++) {
		for (int x = 0; x < ATLAS_WIDTH; x++) {
			int index = y*ATLAS_WIDTH + x;
			rgba8_pixels[index] = 0x00FFFFFF | ((uint32_t)atlas_texture[index] << 24);
		}
	}
	sg_image_desc atlas_image_desc;
	memset(&atlas_image_desc, 0, sizeof(atlas_image_desc));
	atlas_image_desc.width = ATLAS_WIDTH;
	atlas_image_desc.height = ATLAS_HEIGHT;
	/* LINEAR would be better for text quality in HighDPI, but the
	atlas texture is "leaking" from neighbouring pixels unfortunately
	*/
	atlas_image_desc.min_filter = SG_FILTER_NEAREST;
	atlas_image_desc.mag_filter = SG_FILTER_NEAREST;
	atlas_image_desc.data.subimage[0][0].ptr = rgba8_pixels;
	atlas_image_desc.data.subimage[0][0].size = rgba8_size;

	atlas_img = sg_make_image(&atlas_image_desc);

	sg_pipeline_desc pipeline_desc;
	memset(&pipeline_desc, 0, sizeof(pipeline_desc));
	pipeline_desc.colors[0].blend.enabled = true;
	pipeline_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
	pipeline_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

	pip = sgl_make_pipeline(&pipeline_desc);

	free(rgba8_pixels);

	/* setup microui */
	mu_init(&mu_ctx);
	mu_ctx.text_width = text_width_cb;
	mu_ctx.text_height = text_height_cb;
}

void MicroUIRender_shutdown() {
	sgl_shutdown();
}

void r_begin(int width, int height) {
	sgl_defaults();
	sgl_push_pipeline();
	sgl_load_pipeline(pip);
	sgl_enable_texture();
	sgl_texture(atlas_img);
	sgl_matrix_mode_projection();
	sgl_push_matrix();
	sgl_ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, +1.0f);
	sgl_begin_quads();
}

void r_end() {
	sgl_end();
	sgl_pop_matrix();
	sgl_pop_pipeline();
}


static void r_push_quad(mu_Rect dst, mu_Rect src, mu_Color color) {
	float u0 = (float)src.x / (float)ATLAS_WIDTH;
	float v0 = (float)src.y / (float)ATLAS_HEIGHT;
	float u1 = (float)(src.x + src.w) / (float)ATLAS_WIDTH;
	float v1 = (float)(src.y + src.h) / (float)ATLAS_HEIGHT;

	float x0 = (float)dst.x;
	float y0 = (float)dst.y;
	float x1 = (float)(dst.x + dst.w);
	float y1 = (float)(dst.y + dst.h);

	sgl_c4b(color.r, color.g, color.b, color.a);
	sgl_v2f_t2f(x0, y0, u0, v0);
	sgl_v2f_t2f(x1, y0, u1, v0);
	sgl_v2f_t2f(x1, y1, u1, v1);
	sgl_v2f_t2f(x0, y1, u0, v1);
}

static void r_draw_rect(mu_Rect rect, mu_Color color) {
	r_push_quad(rect, atlas[ATLAS_WHITE], color);
}

static void r_draw_text(const char* text, mu_Vec2 pos, mu_Color color) {
	mu_Rect dst = { pos.x, pos.y, 0, 0 };
	for (const char* p = text; *p; p++) {
		mu_Rect src = atlas[ATLAS_FONT + (unsigned char)*p];
		dst.w = src.w;
		dst.h = src.h;
		r_push_quad(dst, src, color);
		dst.x += dst.w;
	}
}

static void r_draw_icon(int id, mu_Rect rect, mu_Color color) {
	mu_Rect src = atlas[id];
	int x = rect.x + (rect.w - src.w) / 2;
	int y = rect.y + (rect.h - src.h) / 2;
	r_push_quad(mu_rect(x, y, src.w, src.h), src, color);
}

static int r_get_text_width(const char* text, int len) {
	int res = 0;
	for (const char* p = text; *p && len--; p++) {
		res += atlas[ATLAS_FONT + (unsigned char)*p].w;
	}
	return res;
}

static int r_get_text_height(void) {
	return 18;
}

static void r_set_clip_rect(mu_Rect rect) {
	sgl_end();
	sgl_scissor_rect(rect.x, rect.y, rect.w, rect.h, true);
	sgl_begin_quads();
}

void MicroUIRender_draw(int width, int height) {

	/* micro-ui rendering */
	r_begin(width, height);
	mu_Command* cmd = 0;
	while (mu_next_command(&mu_ctx, &cmd)) {
		switch (cmd->type) {
		case MU_COMMAND_TEXT: r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color); break;
		case MU_COMMAND_RECT: r_draw_rect(cmd->rect.rect, cmd->rect.color); break;
		case MU_COMMAND_ICON: r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
		case MU_COMMAND_CLIP: r_set_clip_rect(cmd->clip.rect); break;
		}
	}
	r_end();

	sg_pass_action pass_action;
	memset(&pass_action, 0, sizeof(pass_action));

	pass_action.colors[0].action = SG_ACTION_LOAD;

	// clear code ???

	/* render the sokol-gfx default pass */
	sg_begin_default_pass(&pass_action, width, height);
	sgl_draw();
	
	sg_end_pass();
	sg_commit();
}

mu_Context* MicroUIRender_getContext() {
	return &mu_ctx;
}

static int uint8_slider(mu_Context *ctx, unsigned char *value, int low, int high) {
	static float tmp;
	mu_push_id(ctx, &value, sizeof(value));
	tmp = *value;
	int res = mu_slider_ex(ctx, &tmp, low, high, 0, "%.0f", MU_OPT_ALIGNCENTER);
	*value = tmp;
	mu_pop_id(ctx);
	return res;
}

void MicroUIRender_style_window(mu_Context* ctx) {
	static mu_Container window;

	/* init window manually so we can set its position and size */
	if (!window.inited) {
		mu_init_window(ctx, &window, 0);
		window.rect = mu_rect(350, 250, 300, 240);
	}

	static struct { const char *label; int idx; } colors[] = {
		{ "text:",         MU_COLOR_TEXT },
		{ "border:",       MU_COLOR_BORDER },
		{ "windowbg:",     MU_COLOR_WINDOWBG },
		{ "titlebg:",      MU_COLOR_TITLEBG },
		{ "titletext:",    MU_COLOR_TITLETEXT },
		{ "panelbg:",      MU_COLOR_PANELBG },
		{ "button:",       MU_COLOR_BUTTON },
		{ "buttonhover:",  MU_COLOR_BUTTONHOVER },
		{ "buttonfocus:",  MU_COLOR_BUTTONFOCUS },
		{ "base:",         MU_COLOR_BASE },
		{ "basehover:",    MU_COLOR_BASEHOVER },
		{ "basefocus:",    MU_COLOR_BASEFOCUS },
		{ "scrollbase:",   MU_COLOR_SCROLLBASE },
		{ "scrollthumb:",  MU_COLOR_SCROLLTHUMB },
		{ NULL }
	};

	if (mu_begin_window(ctx, &window, "Style Editor")) {
		int sw = mu_get_container(ctx)->body.w * 0.14;
		mu_layout_row(ctx, 6, (int[]) { 80, sw, sw, sw, sw, -1 }, 0);
		for (int i = 0; colors[i].label; i++) {
			mu_label(ctx, colors[i].label);
			uint8_slider(ctx, &ctx->style->colors[i].r, 0, 255);
			uint8_slider(ctx, &ctx->style->colors[i].g, 0, 255);
			uint8_slider(ctx, &ctx->style->colors[i].b, 0, 255);
			uint8_slider(ctx, &ctx->style->colors[i].a, 0, 255);
			mu_draw_rect(ctx, mu_layout_next(ctx), ctx->style->colors[i]);
		}
		mu_end_window(ctx);
	}
}
