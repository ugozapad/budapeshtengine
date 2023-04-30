-- Семантика входного формата вершин в шейдер должна иметь DX11 имена,
-- на GL строка превратится в ловер-кейс.

-- Устанавливаем использование только 16-ти битных индексов
matsys.force_16bit_indices(true)

-- Движковые, шаблонные материалы.
materials = {}

-- Лайтмапа обычная
materials["lightmapped_generic"] = {

	-- Формат вершин позиция + нормаль + текстурнные 0, 1
	-- (такой же как и у материалов с лайтмапой и освещением )
	vertex_fomat = {
		{ "POSITION", "vec3", 0 },
		{ "NORMAL", "vec3", 1 },
		{ "TEXCOORD0", "vec2", 2 },
		{ "TEXCOORD1", "vec2", 3 },
	},
	
	vertex_shader_name = "lightmapped_generic.vs",
	fragment_shader_name = "lightmapped_generic.fs"
}

-- Лайтмапа прозрачная
materials["lightmapped_trans"] = {

	-- Формат вершин позиция + нормаль + текстурнные 0, 1
	-- (такой же как и у материалов с лайтмапой и освещением )
	vertex_fomat = {
		{ "POSITION", "vec3", 0 },
		{ "NORMAL", "vec3", 1 },
		{ "TEXCOORD0", "vec2", 2 },
		{ "TEXCOORD1", "vec2", 3 },
	},
	
	vertex_shader_name = "lightmapped_generic.vs",
	fragment_shader_name = "lightmapped_generic.fs"
}

-- Лайтмапа + 1 точечный источник света
materials["lightmapped_light"] = {

	-- Формат вершин позиция + нормаль + текстурнные 0, 1
	vertex_fomat = {
		{ "POSITION", "vec3", 0 },
		{ "NORMAL", "vec3", 1 },
		{ "TEXCOORD0", "vec2", 2 },
		{ "TEXCOORD1", "vec2", 3 },
	},
	
	vertex_shader_name = "lightmapped_light.vs",
	fragment_shader_name = "lightmapped_light.fs",
	
	fs_uniforms = {
		{ "u_light_color", "vec3" },
		{ "u_light_radius", "float" }
		
		-- квадратичное, линейное затухание ???
	}
}

-- материал неба, три прохода
materials["lightmapped_light"] = {

	-- Формат вершин
	vertex_fomat = {
		{ "POSITION", "vec3", 0 },
		{ "NORMAL", "vec3", 1 },
		{ "TEXCOORD", "vec2", 2 },
	}
	
	--passes = {
	--
	--	-- рисуем базовую модель
	--	{
	--		vertex_shader_name = "model_base.vs",
	--		fragment_shader_name = "model_base.fs",
	--		
	--
	--		
	--	}
	--
	--}
	
}

-- алиасы внутреннего имени материала к дизайнерскому
material_aliases = {
	def_lm = lightmapped_generic,
	default_lm = lightmapped_generic,
	def_trans = lightmapped_trans,
	default_trans = lightmapped_trans,
}