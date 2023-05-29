#ifndef MATERIAL_SYSTEM_H
#define MATERIAL_SYSTEM_H

class Texture;

class MaterialSystem {
public:
	MaterialSystem();
	~MaterialSystem();

	void Init();
	void Shutdown();

	Texture* LoadTexture(const char* filename);

private:
	Texture* m_notex;
};

extern MaterialSystem g_material_system;

#endif // !MATERIAL_SYSTEM_H
