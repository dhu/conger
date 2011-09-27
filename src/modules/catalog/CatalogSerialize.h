#ifndef CATALOGSERIALIZE_H
#define CATALOGSERIALIZE_H

#include "common.h"

BOREALIS_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
//
// Pointer serialization
//

template<typename T>
::NMSTL::OSerial& freeze_ptr_impl(::NMSTL::OSerial &out, T* field)
{
    if (field)
    {
    out.begin_container(*field);
    out << field->get_typeid() << *field;
    out.end_container(*field);
    }
    else
    {
    out.begin_container(*field);
    out << typename T::typeid_type();
    out.end_container(*field);
    }

    return out;
}


template<typename T>
::NMSTL::OSerial& freeze_ptr_shallow_impl(::NMSTL::OSerial &out, T* field)
{
    if (field)
    {
    out.begin_container(*field);
    out << field->get_typeid();
    field->shallow_freeze(out);
    out.end_container(*field);
    }
    else
    {
    out.begin_container(*field);
    out << typename T::typeid_type();
    out.end_container(*field);
    }

    return out;
}


////////////////////////////////////////////////////////////////////////////////
//
// Pointer deserialization
//

template<typename T>
::NMSTL::ISerial& unfreeze_ptr_impl(::NMSTL::ISerial& in, T*& field)
{
    in.begin_container(*field);
    field = __nmstl_serial_create(serial_surrogate<T>(), in);
    in.end_container(*field);

    return in;
}


////////////////////////////////////////////////////////////////////////////////
//
// STL Containers
//
#ifdef ENABLE_CONTAINER_SERIALIZATION

template<typename Container>
void freeze_ptr_impl_container(::NMSTL::OSerial& out, const Container& field)
{
    out.write_type(typeid(Container), field.size());
    //    if (out.get_mode() == ::NMSTL::ISerial::typesafe
    //  || out.get_mode() == ::NMSTL::ISerial::readable)
    //  out.put('{');
    //    else
    if (out.get_mode() == ::NMSTL::ISerial::xml)
    {
        typename Container::value_type c;
        out.begin_container(c);
    }

    for (typename Container::const_iterator i = field.begin(); i != field.end(); ++i)
    freeze_ptr_impl(out, *i);

    //if (out.get_mode() == ::NMSTL::ISerial::typesafe
    //  || out.get_mode() == ::NMSTL::ISerial::readable)
    //  out.put('}');
    //else
    if (out.get_mode() == ::NMSTL::ISerial::xml)
    {
    typename Container::value_type c;
    out.end_container(c);
    }
}

template<typename T>
inline ::NMSTL::OSerial& freeze_ptr_impl(::NMSTL::OSerial& out, const vector<T>& t)
{
    freeze_ptr_impl_container(out,t);
    return out;
}

template<typename T, typename U>
inline ::NMSTL::OSerial& freeze_ptr_impl(::NMSTL::OSerial& out, const map<T,U>& t)
{
    freeze_ptr_impl_container(out,t);
    return out;
}

template<typename T>
inline ::NMSTL::OSerial& freeze_ptr_impl(::NMSTL::OSerial& out, const set<T>& t)
{
    freeze_ptr_impl_container(out,t);
    return out;
}



////////////////////////////////////////////////////////////////////////////////
//
// STL Containers (shallow serialization)
//
template<typename Container>
void freeze_ptr_shallow_impl_container(::NMSTL::OSerial& out, const Container& field)
{
    out.write_type(typeid(Container), field.size());
    //if (out.get_mode() == ::NMSTL::ISerial::typesafe
    //|| out.get_mode() == ::NMSTL::ISerial::readable)
    //out.put('{');
    //else
    if (out.get_mode() == ::NMSTL::ISerial::xml)
    {
        typename Container::value_type c;
        out.begin_container(c);
    }

    for (typename Container::const_iterator i = field.begin(); i != field.end(); ++i)
    freeze_ptr_shallow_impl(out, *i);

    //if (out.get_mode() == ::NMSTL::ISerial::typesafe
    //|| out.get_mode() == ::NMSTL::ISerial::readable)
    //out.put('}');
    //else
    if (out.get_mode() == ::NMSTL::ISerial::xml)
    {
    typename Container::value_type c;
    out.end_container(c);
    }
}


template<typename T>
inline ::NMSTL::OSerial& freeze_ptr_shallow_impl(::NMSTL::OSerial& out, const vector<T>& t)
{
    freeze_ptr_shallow_impl_container(out, t);
    return out;
}

template<typename T, typename U>
inline ::NMSTL::OSerial& freeze_ptr_shallow_impl(::NMSTL::OSerial& out, const map<T,U>& t)
{
    freeze_ptr_shallow_impl_container(out, t);
    return out;
}

template<typename T>
inline ::NMSTL::OSerial& freeze_ptr_shallow_impl(::NMSTL::OSerial& out, const set<T>& t)
{
    freeze_ptr_shallow_impl_container(out, t);
    return out;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Pointer containers deserialization
//
template<typename Container, typename data>
inline void unfreeze_ptr_impl_container(::NMSTL::ISerial& in, Container& field)
{
    field.clear();

    DEBUG << "Reading " << to_string(typeid(Container));

    if (in.get_mode() == ::NMSTL::ISerial::xml)
    {
    data item;
    in.begin_container(item);

    while (in)
    {
        string s = in.get_tokens()->peek_tag();
        if (s != "value")
        break;

        if (!(unfreeze_ptr_impl(in, item))) break;
        field.insert(field.end(), item);
    }

    in.end_container(item);
    return;
    }

    unsigned int length;
    if (!in.check_type(typeid(Container), length)) return;

    //if ((in.get_mode() == ::NMSTL::ISerial::typesafe
    //|| in.get_mode() == ::NMSTL::ISerial::readable) && in.get() != '{')
    //{
    //in.set_error("Failed to read brace starting container");
    //return;
    //}

    data item;
    for (unsigned int i = 0; i < length; ++i)
    {
    if (!(unfreeze_ptr_impl(in, item))) break;
    field.insert(field.end(), item);
    }

    //if ((in.get_mode() == ::NMSTL::ISerial::typesafe
    //|| in.get_mode() == ::NMSTL::ISerial::readable) && in.get() != '}')
    //{
    //in.set_error("Failed to read brace ending container");
    //return;
    //}
}

template<typename T>
inline ::NMSTL::ISerial& unfreeze_ptr_impl(::NMSTL::ISerial& in, vector<T>& t)
{
    unfreeze_ptr_impl_container<vector<T>,T>(in, t);
    return in;
}

template<typename T, typename U>
inline ::NMSTL::ISerial& unfreeze_ptr_impl(::NMSTL::ISerial& in, map<T,U>& t)
{
    unfreeze_ptr_impl_container< map<T,U>, pair<T,U> >(in, t);
    return in;
}

template<typename T>
inline ::NMSTL::ISerial& unfreeze_ptr_impl(::NMSTL::ISerial& in, set<T>& t)
{
    unfreeze_ptr_impl_container<set<T>,T>(in, t);
    return in;
}

#endif // ENABLE_CONTAINER_SERIALIZATION

BOREALIS_NAMESPACE_END;

#endif // CATALOGSERIALIZE_H
