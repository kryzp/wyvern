#ifndef LIGHT_H_
#define LIGHT_H_

#include <wvn/maths/colour.h>
#include <wvn/maths/quat.h>
#include <wvn/maths/vec3.h>

namespace wvn::gfx
{
	class RenderTarget;
	class TextureSampler;

	enum LightType
	{
		LIGHT_TYPE_NONE,
		LIGHT_TYPE_POINT,
		LIGHT_TYPE_DIR,
		LIGHT_TYPE_SPOT,
		LIGHT_TYPE_MAX_ENUM
	};

	using LightID = u32;

	class LightHandle;

	class Light
	{
		friend class LightHandle;

	public:
		constexpr static LightID NULL_ID = 0;

		constexpr static u32 DEPTH_TEXTURE_WIDTH = 2048;
		constexpr static u32 DEPTH_TEXTURE_HEIGHT = 2048;

		Light(LightID id);
		~Light();

		LightType get_type() const;
		void set_type(LightType type);

		const Colour& get_colour() const;
		void set_colour(const Colour& colour);

		bool is_ambient_enabled() const;
		void toggle_ambient(bool enabled);

		const Colour& get_ambient() const;
		void set_ambient(const Colour& colour);

		bool is_shadow_caster() const;
		void toggle_shadows(bool enabled);

		RenderTarget* get_shadow_map() const;
		void set_shadow_map(RenderTarget* target);

		float get_shadow_near_clip() const;
		void set_shadow_near_clip(float distance);

		float get_shadow_far_clip() const;
		void set_shadow_far_clip(float distance);

		float get_falloff() const;
		void set_falloff(float falloff);

		float get_intensity() const;
		void set_intensity(float intensity);

		const Quat& get_orientation() const;
		void set_orientation(const Quat& orientation);

		const Vec3F& get_position() const;
		void set_position(const Vec3F& position);

	private:
		LightType m_type;
		LightID m_id;

		bool m_ambient_enabled;

		Colour m_colour;
		Colour m_ambient;

		bool m_shadows_enabled;
		float m_shadow_near_clip;
		float m_shadow_far_clip;
		RenderTarget* m_shadow_map;

		float m_intensity;
		float m_falloff;

		Quat m_orientation;
		Vec3F m_position;
	};

	class LightHandle
	{
	public:
		LightHandle();
		LightHandle(const Light* object);
		LightHandle(LightID id);
		LightHandle(const LightHandle& other);
		LightHandle(LightHandle&& other) noexcept;
		LightHandle& operator = (const LightHandle& other);
		LightHandle& operator = (LightHandle&& other) noexcept;
		~LightHandle() = default;

		bool is_valid() const;
		operator bool () const;

		LightID id() const;
		operator LightID () const;

		Light* get();
		const Light* get() const;

		Light* operator -> ();
		const Light* operator -> () const;

		bool operator == (const LightHandle& other) const;
		bool operator != (const LightHandle& other) const;

	private:
		LightID m_id;
	};

	/*
	struct ShadowCaster
	{
		const Light* light;
		RenderTarget* shadow_map;
	};

	class LightShadowMapMgr
	{
	public:
		constexpr static unsigned MAX_SHADOW_CASTERS = 512;

		LightShadowMapMgr();
		~LightShadowMapMgr();

		void init();

		Vector<ShadowCaster> get_shadow_casters() const;

	private:
		void create_depth_texture();

		RenderTarget* m_light_shadow_textures[MAX_SHADOW_CASTERS];
		int m_light_shadow_texture_num;
	};
	*/
}

#endif // LIGHT_H_
