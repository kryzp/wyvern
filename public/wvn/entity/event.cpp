#include <wvn/entity/event.h>
#include <wvn/entity/event_mgr.h>
#include <wvn/entity/entity.h>

using namespace wvn;
using namespace wvn::ent;

Event::Event()
	: type()
	, args()
	, receiver()
	, handled(false)
{
}

Event::Event(const String& type)
	: type(type)
	, args()
	, receiver()
	, handled(false)
{
}

Event::Event(const String& type, const Args& args, const EntityHandle& recv)
	: type(type)
	, args(args)
	, receiver(recv)
	, handled(false)
{
}

void Event::send(const EntityHandle& handle)
{
	wvn_ASSERT(handle.is_valid(), "[ACT:EVENT|DEBUG] Receiver of event must be valid.");

    receiver = handle;
	EventMgr::get_singleton()->enqueue_event(*this);
}

void Event::send(const Entity* entity)
{
	EntityHandle handle(entity);
	send(handle);
}

bool Event::dispatch()
{
	handled = receiver->on_event(*this);
	return handled;
}

bool Event::is_type(const char* other) const
{
	return hash::calc(other) == hash::calc(&type);
}

u64 Event::type_hash() const
{
	return hash::calc(type.c_str());
}

void Event::append(const String& name, const EventArg& val)
{
	args.insert(Pair(name, val));
}

void Event::append_s8(const String& name, s8 val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_S8;
	eval.data.s8 = val;
	args.insert(Pair(name, eval));
}

void Event::append_s16(const String& name, s16 val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_S16;
	eval.data.s16 = val;
	args.insert(Pair(name, eval));
}

void Event::append_s32(const String& name, s32 val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_S32;
	eval.data.s32 = val;
	args.insert(Pair(name, eval));
}

void Event::append_s64(const String& name, s64 val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_S64;
	eval.data.s64 = val;
	args.insert(Pair(name, eval));
}

void Event::append_u8(const String& name, u8 val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_U8;
	eval.data.u8 = val;
	args.insert(Pair(name, eval));
}

void Event::append_u16(const String& name, u16 val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_U16;
	eval.data.u16 = val;
	args.insert(Pair(name, eval));
}

void Event::append_u32(const String& name, u32 val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_U32;
	eval.data.u32 = val;
	args.insert(Pair(name, eval));
}

void Event::append_u64(const String& name, u64 val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_U64;
	eval.data.u64 = val;
	args.insert(Pair(name, eval));
}

void Event::append_f32(const String& name, f32 val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_F32;
	eval.data.f32 = val;
	args.insert(Pair(name, eval));
}

void Event::append_f64(const String& name, f64 val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_F64;
	eval.data.f64 = val;
	args.insert(Pair(name, eval));
}

void Event::append_bool(const String& name, bool val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_BOOL;
	eval.data.boolean = val;
	args.insert(Pair(name, eval));
}

void Event::append_char(const String& name, char val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_CHAR;
	eval.data.character = val;
	args.insert(Pair(name, eval));
}

void Event::append_str(const String& name, const char* val)
{
	EventArg eval;
	eval.type = EventArg::ARG_TYPE_STRING;
	eval.data.string = val;
	args.insert(Pair(name, eval));
}
