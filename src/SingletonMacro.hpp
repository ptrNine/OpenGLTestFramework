#pragma once

#define MARK_AS_SINGLETON(CLASSNAME) \
public: \
    CLASSNAME(const CLASSNAME&) = delete; \
    CLASSNAME& operator=(const CLASSNAME&) = delete; \
    static CLASSNAME& instance() { static CLASSNAME inst; return inst; } \
protected: \
    CLASSNAME(); \
    ~CLASSNAME()


#define DEFINE_GETTER(field_name, getter_name) \
auto& getter_name() { return field_name; } \
auto& getter_name() const { return field_name; }

#define DEFINE_CONST_GETTER(field_name, getter_name) \
auto& getter_name() const { return field_name; }
