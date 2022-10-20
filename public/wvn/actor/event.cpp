#include <wvn/actor/event.h>
#include <wvn/actor/event_mgr.h>

#include <wvn/actor/actor.h>

using namespace wvn;
using namespace wvn::act;

void Event::send(const ActorHandle& handle)
{
	WVN_ASSERT(handle.is_valid(), "[EVENT:DEBUG] Receiver of event must be valid.");

	this->receiver = handle;

	Event* dcopy = deep_copy();
	EventMgr::get_singleton().enqueue_event(dcopy);
}

void Event::send(const Actor* actor)
{
	ActorHandle handle(actor);
	send(handle);
}

void Event::dispatch()
{
	receiver->on_event(*this);
}

Event* Event::deep_copy()
{
	Event* dcopy = new Event();
	dcopy->type = this->type;
	dcopy->args = this->args;
	dcopy->receiver = this->receiver;

	return dcopy;
}

u64 Event::type_hash() const
{
	return SID(type.c_str());
}

void Event::append(const String& name, const EventValue& val)
{
	this->args.insert(Pair(name, val));
}

void Event::append_s8(const String& name, s8 val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_S8;
	eval.s8 = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_s16(const String& name, s16 val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_S16;
	eval.s16 = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_s32(const String& name, s32 val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_S32;
	eval.s32 = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_s64(const String& name, s64 val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_S64;
	eval.s64 = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_u8(const String& name, u8 val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_U8;
	eval.u8 = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_u16(const String& name, u16 val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_U16;
	eval.u16 = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_u32(const String& name, u32 val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_U32;
	eval.u32 = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_u64(const String& name, u64 val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_U64;
	eval.u64 = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_f32(const String& name, f32 val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_F32;
	eval.f32 = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_f64(const String& name, f64 val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_F64;
	eval.f64 = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_bool(const wvn::String& name, bool val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_BOOL;
	eval.boolean = val;

	this->args.insert(Pair(name, eval));
}

void Event::append_char(const String& name, char val)
{
	EventValue eval;
	eval.type = EventValue::ARG_TYPE_CHAR;
	eval.character = val;

	this->args.insert(Pair(name, eval));
}
