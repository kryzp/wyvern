#ifndef SHADER_H_
#define SHADER_H_

#include <wvn/common.h>
#include <wvn/container/vector.h>
#include <wvn/container/hash_map.h>
#include <wvn/container/string.h>
#include <wvn/maths/mat4x4.h>
#include <wvn/maths/affine_3d.h>

namespace wvn::gfx
{
	/**
	 * Different types of shader.
	 */
	enum ShaderProgramType
	{
		SHADER_TYPE_NONE = -1,

		SHADER_TYPE_VERTEX		= 0,
		SHADER_TYPE_FRAGMENT	= 1,
		SHADER_TYPE_GEOMETRY	= 2,
		SHADER_TYPE_COMPUTE		= 3,

		SHADER_TYPE_MAX_ENUM,

		SHADER_TYPE_GRAPHICS_COUNT = 2 // todo: badddd
	};

	/**
	 * Represents the possible parameters that can be passed into a shader.
	 */
	struct ShaderParameter
	{
		static constexpr u64 MAX_PARAMETER_SIZE = sizeof(float) * 64;

		enum ParameterType
		{
			PARAM_TYPE_NONE,

			PARAM_TYPE_S8,
			PARAM_TYPE_S16,
			PARAM_TYPE_S32,
			PARAM_TYPE_S64,

			PARAM_TYPE_U8,
			PARAM_TYPE_U16,
			PARAM_TYPE_U32,
			PARAM_TYPE_U64,

			PARAM_TYPE_F32,
			PARAM_TYPE_F64,
			PARAM_TYPE_BOOL,

			PARAM_TYPE_VEC2F,
			PARAM_TYPE_VEC3F,
			PARAM_TYPE_VEC4F,

//			PARAM_TYPE_MAT3X3F,
			PARAM_TYPE_MAT4X4F,

			PARAM_TYPE_MAX_ENUM
		};

		ParameterType type;
		byte data[MAX_PARAMETER_SIZE];

		constexpr u64 alignment_offset() const
		{
			switch (type)
			{
				case PARAM_TYPE_S8:			return 4;
				case PARAM_TYPE_S16:		return 4;
				case PARAM_TYPE_S32:		return 8;
				case PARAM_TYPE_S64:		return 16;

				case PARAM_TYPE_U8:			return 4;
				case PARAM_TYPE_U16:		return 4;
				case PARAM_TYPE_U32:		return 8;
				case PARAM_TYPE_U64:		return 16;

				case PARAM_TYPE_F32:		return 4;
				case PARAM_TYPE_F64:		return 8;

				case PARAM_TYPE_BOOL:		return 4;

				case PARAM_TYPE_VEC2F:		return 8;
				case PARAM_TYPE_VEC3F:		return 16;
				case PARAM_TYPE_VEC4F:		return 16;

//				case PARAM_TYPE_MAT3X3F:	return 64;
				case PARAM_TYPE_MAT4X4F:	return 64;

				default: return 0;
			}
		}

		constexpr u64 size() const
		{
			switch (type)
			{
				case PARAM_TYPE_S8:			return sizeof(s8);
				case PARAM_TYPE_S16:		return sizeof(s16);
				case PARAM_TYPE_S32:		return sizeof(s32);
				case PARAM_TYPE_S64:		return sizeof(s64);

				case PARAM_TYPE_U8:			return sizeof(u8);
				case PARAM_TYPE_U16:		return sizeof(u16);
				case PARAM_TYPE_U32:		return sizeof(u32);
				case PARAM_TYPE_U64:		return sizeof(u64);

				case PARAM_TYPE_F32:		return sizeof(float);
				case PARAM_TYPE_F64:		return sizeof(double);

				case PARAM_TYPE_BOOL:		return sizeof(bool);

				case PARAM_TYPE_VEC2F:		return sizeof(float) * 2;
				case PARAM_TYPE_VEC3F:		return sizeof(float) * 3;
				case PARAM_TYPE_VEC4F:		return sizeof(float) * 4;

//				case PARAM_TYPE_MAT3X3F:	return sizeof(float) * 9;
				case PARAM_TYPE_MAT4X4F:	return sizeof(float) * 16;

				default: return 0;
			}
		}
	};

	struct Vec4F { float r,g,b,a; };

	/**
	 * Represents a list of a shader program parameters
	 */
	class ShaderParameters
	{
	public:
		using PackedConstants = Vector<byte>;

		ShaderParameters() = default;
		~ShaderParameters() = default;

		const PackedConstants& get_packed_constants();

		void reset()
		{
			m_constants.clear();
			m_packed_constants.clear();
			m_dirty_constants = true;
		}

		void set(const String& name, s8 val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_S8); }
		void set(const String& name, s16 val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_S16); }
		void set(const String& name, s32 val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_S32); }
		void set(const String& name, s64 val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_S64); }
		void set(const String& name, u8 val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_U8); }
		void set(const String& name, u16 val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_U16); }
		void set(const String& name, u32 val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_U32); }
		void set(const String& name, u64 val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_U64); }
		void set(const String& name, f32 val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_F32); }
		void set(const String& name, f64 val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_F64); }
		void set(const String& name, bool val)				{ _set(name, val, ShaderParameter::PARAM_TYPE_BOOL); }
		void set(const String& name, const Vec2F& val)		{ _set(name, val, ShaderParameter::PARAM_TYPE_VEC2F); }
		void set(const String& name, const Vec3F& val)		{ _set(name, val, ShaderParameter::PARAM_TYPE_VEC3F); }
		void set(const String& name, const Vec4F& val)		{ _set(name, val, ShaderParameter::PARAM_TYPE_VEC4F); }
//		void set(const String& name, const Basis3D& val)	{ _set(name, val, ShaderParameter::PARAM_TYPE_MAT3X3F); }
		void set(const String& name, const Mat4x4& val)		{ _set(name, val, ShaderParameter::PARAM_TYPE_MAT4X4F); }

	private:
		template <typename T>
		void _set(const String& name, const T& val, ShaderParameter::ParameterType type)
		{
			int i = 0;

			for (; i <= m_constants.size(); i++) {
				if (i == m_constants.size()) {
					break;
				}

				if (m_constants[i].first == name) {
					break;
				}
			}

			if (i < m_constants.size()) {
				mem::copy(m_constants[i].second.data, &val, sizeof(T));
			} else {
				ShaderParameter p = {};
				p.type = type;
				mem::copy(p.data, &val, sizeof(T));
				m_constants.push_back(Pair(name, p));
			}

			m_dirty_constants = true;
		}

		void rebuild_packed_constant_data();

		Vector<Pair<String, ShaderParameter>> m_constants; // the reason we don't use a hashmap here is because we need to PRESERVE THE ORDER of the elements! the hashmap inherently is unordered.
		bool m_dirty_constants;
		PackedConstants m_packed_constants;
	};

	/**
	 * Describes a GPU shader program, such as the vertex or fragment shader.
	 */
	class ShaderProgram
	{
	public:
		ShaderProgram();
		virtual ~ShaderProgram();

		virtual void clean_up() = 0;
		virtual void load_from_source(const char* source, u64 source_size) = 0;

		ShaderProgramType type;
		ShaderParameters params;
	};

	/**
	 * Describes a collection of GPU shader programs, making up a full "effect", essentially
	 * the pipeline of vertex -> ... -> fragment, all in stages.
	 */
	class ShaderEffect
	{
	public:
		ShaderEffect();
		~ShaderEffect();

		void add_stage(ShaderProgram* shader) { stages.push_back(shader); }

		Vector<ShaderProgram*> stages;
	};
}

#endif // SHADER_H_
