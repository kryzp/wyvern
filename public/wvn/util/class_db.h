#ifndef CLASS_DB_H
#define CLASS_DB_H

#include <wvn/container/string.h>
#include <wvn/container/vector.h>

/**
 * So far this is being unused.
 * At some point I'll add it.
 */

namespace wvn
{
	class ClassDB
	{
	public:
		template <class C>
		static void add_class()
		{
		}
	};
}

#define WVNCLASS_BASE(_class) 						\
private:											\
	friend class ::wvn::ClassDB;					\
public:												\
	virtual String get_class() const {   			\
		return String(#_class);             		\
	}                                    			\
	static String get_class_static() { 				\
		return String(#_class);              		\
	}                 								\
    virtual bool is_class(const String& c) const { 	\
    	return String(#_class) == c;                \
	}                                    			\
	static void init_class() {           			\
    	static bool is_initialized = false;   		\
		if (is_initialized) {               		\
			return;                             	\
		}                                   		\
		::wvn::ClassDB::add_class<_class>();		\
		is_initialized = true; 						\
	} 												\
private:

#define WVNCLASS(_class, _inherits)										\
private:																\
	friend class ::wvn::ClassDB;          								\
public:                               									\
	virtual String get_class() const {   								\
		return String(#_class);             							\
	}                                    								\
	static String get_class_static() { 									\
		return String(#_class);              							\
	}                                    								\
	static String get_parent_class_static() { 							\
		return _inherits::get_class_static(); 							\
	}                                    								\
    virtual bool is_class(const String& c) const { 						\
    	return (c == String(#_class)) ? true : _inherits::is_class(c);  \
	}                                    								\
    static void get_inheritance_list_static(Vector<String>* list) { 	\
		_inherits::get_inheritance_list_static(list);    				\
		list->push_back(String(#_class));								\
	} 																	\
	static void init_class() {           								\
    	static bool is_initialized = false;   							\
		if (is_initialized) {               							\
			return;                             						\
		}                                   							\
		_inherits::init_class();            							\
		::wvn::ClassDB::add_class<_class>();							\
		is_initialized = true; 											\
	} 																	\
private:

#endif // CLASS_DB_H
