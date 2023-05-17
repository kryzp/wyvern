#ifndef EVENT_H
#define EVENT_H

#include <wvn/common.h>
#include <wvn/container/hash_map.h>
#include <wvn/container/string.h>
#include <wvn/actor/actor.h>
#include <wvn/actor/actor_handle.h>

namespace wvn::act
{
	/**
	 * Represents a value that can be passed into an event.
	 */
	struct EventArg
	{
		enum ArgType
		{
			ARG_TYPE_NULL = 0,

			ARG_TYPE_S8,
			ARG_TYPE_S16,
			ARG_TYPE_S32,
			ARG_TYPE_S64,

			ARG_TYPE_U8,
			ARG_TYPE_U16,
			ARG_TYPE_U32,
			ARG_TYPE_U64,

			ARG_TYPE_F32,
			ARG_TYPE_F64,

			ARG_TYPE_BOOL,
			ARG_TYPE_CHAR,

			ARG_TYPE_STRING
		};

		ArgType type = ARG_TYPE_NULL;

		union
		{
			s8 s8;
			s16 s16;
			s32 s32;
			s64 s64;

			u8 u8;
			u16 u16;
			u32 u32;
			u64 u64;

			f32 f32;
			f64 f64;

			bool b8;
			char c8;

			const char* string;
		}
		data;
	};

	class EventMgr;

	/**
	 * Event that can be passed to the event
	 * manager which dispatches it.
	 */
	class Event
	{
		friend class EventMgr;

	public:
		using Args = HashMap<String, EventArg>;

		Event();
		Event(const String& type);
		Event(const String& type, const Args& args, const ActorHandle& recv);

		void send(const ActorHandle& handle);
		void send(const Actor* actor);
		bool dispatch();

		bool is_type(const char* other) const;
		u64 type_hash() const;

		void append(const String& name, const EventArg& val);

		void append_s8(const String& name, s8 val);
		void append_s16(const String& name, s16 val);
		void append_s32(const String& name, s32 val);
		void append_s64(const String& name, s64 val);

		void append_u8(const String& name, u8 val);
		void append_u16(const String& name, u16 val);
		void append_u32(const String& name, u32 val);
		void append_u64(const String& name, u64 val);

		void append_f32(const String& name, f32 val);
		void append_f64(const String& name, f64 val);

		void append_bool(const String& name, bool val);
		void append_char(const String& name, char val);

		void append_str(const String& name, const char* val);

		String type;
		Args args;
		ActorHandle receiver;
		bool handled;
	};
}

#endif // EVENT_H
