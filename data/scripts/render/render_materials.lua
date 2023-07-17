r_materials = {}


r_materials["default_lightmap"] = {
	pass = {}
	
	-- default one pass
	pass["generic"] = {
		vertex_shader_name = "lightmapped_generic.vs",
		pixel_shader_name = "lightmapped_generic.fs",
		
		input_layout = {
			{ "float3", "position" },
			{ "float2", "texcoord0" },
			{ "float2", "texcoord1" },
		},
		
		render_state = {
			allowed_to_overwrite = true,
		},
	},
	
	-- first (generic) of two passes lightmaped surfaces rendering
	--     for color + diffuse * detail or diffuse * dyn light
	pass["generic_only_color"] = {
		shader_defines = { "LIGHTMAP_ONLY_COLOR" }
	
		vertex_shader_name = "lightmapped_generic.vs",
		pixel_shader_name = "lightmapped_generic.fs",
		
		input_layout = {
			{ "float3", "position" },
			{ "float2", "texcoord0" },
			{ "float2", "texcoord1" },
		},
		
		render_state = {
			allowed_to_overwrite = false,
			-- additive blending
			--blend_src = "ONE",
			--blend_dst = "ONE",
			--depth_func = "lequal",
		},
	},
}