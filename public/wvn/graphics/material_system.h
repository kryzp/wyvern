#ifndef MATERIAL_MGR
#define MATERIAL_MGR

#include <wvn/singleton.h>
#include <wvn/container/vector.h>
#include <wvn/container/hash_map.h>
#include <wvn/container/string.h>
#include <wvn/graphics/material.h>
#include <wvn/graphics/shader.h>

namespace wvn::gfx
{
	/**
	 * Responsible for the management of materials.
	 */
	class MaterialSystem : public Singleton<MaterialSystem>
	{
		wvn_DEF_SINGLETON(MaterialSystem);

	public:
		MaterialSystem();
		~MaterialSystem();

		void load_default_techniques();

		Material* build_material(const MaterialData& data);
		void add_technique(const String& name, const Technique& technique);

	private:
		Vector<Material*> m_materials;
		HashMap<String, Technique> m_techniques;
	};
}

#endif // MATERIAL_MGR
