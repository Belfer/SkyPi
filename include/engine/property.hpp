#pragma once

// This class is meant to be a way to reflect the memember of a class without using macros
// WIP this currently doesn't achieve it's goal.

#include <functional>

/// Utility for functions get, set & ptr.
template<typename TVal>
using GetFn = std::function<const TVal& (void)>;

template<typename TVal>
using SetFn = std::function<void(const TVal&)>;

template<typename TVal>
using PtrFn = std::function<TVal* (void)>;

/// The property class and each specialization utility.
template<typename TVal, bool Delegate, bool ReadOnly>
class Property;

template<typename TVal>
using PropertyGetSet = Property<TVal, false, false>;

template<typename TVal>
using PropertyDelGetSet = Property<TVal, true, false>;

template<typename TVal>
using PropertyGet = Property<TVal, false, true>;

template<typename TVal>
using PropertyDelGet = Property<TVal, true, true>;

/// <summary>
/// Property get-set.
/// </summary>
/// <typeparam name="TVal">Value type.</typeparam>
template<typename TVal>
class Property<TVal, false, false>
{
public:
    typedef TVal Value;

    Property(const TVal& val)
        : m_value(val)
    {}

    inline const TVal& Get() const { return m_value; }
    inline void Set(const TVal& val) { m_value = val; }
    inline TVal* Ptr() { return &m_value; }

private:
    TVal m_value;
};

/// <summary>
/// Property delegate get-set.
/// </summary>
/// <typeparam name="TVal">Value type.</typeparam>
template<typename TVal>
class Property<TVal, true, false>
{
public:
    typedef TVal Value;

    Property(GetFn<TVal> getFn, SetFn<TVal> setFn)
        : m_getFn(getFn)
        , m_setFn(setFn)
    {}

    Property(GetFn<TVal> getFn, SetFn<TVal> setFn, PtrFn<TVal> ptrFn)
        : m_getFn(getFn)
        , m_setFn(setFn)
        , m_ptrFn(ptrFn)
    {}

    inline const TVal& Get() const { return m_getFn(); }
    inline void Set(const TVal& val) { m_setFn(val); }
    inline TVal* Ptr() { return m_ptrFn(); }

private:
    GetFn<TVal> m_getFn;
    SetFn<TVal> m_setFn;
    PtrFn<TVal> m_ptrFn;
};

/// <summary>
/// Property get.
/// </summary>
/// <typeparam name="TVal">Value type.</typeparam>
template<typename TVal>
class Property<TVal, false, true>
{
public:
    typedef TVal Value;

    Property(const TVal& val)
        : m_value(val)
    {}

    inline const TVal& Get() const { return m_value; }
    inline TVal* Ptr() { return &m_value; }

private:
    TVal m_value;
};

/// <summary>
/// Property delegate get.
/// </summary>
/// <typeparam name="TVal">Value type.</typeparam>
template<typename TVal>
class Property<TVal, true, true>
{
public:
    typedef TVal Value;

    Property(GetFn<TVal> getFn)
        : m_getFn(getFn)
    {}

    Property(GetFn<TVal> getFn, PtrFn<TVal> ptrFn)
        : m_getFn(getFn)
        , m_ptrFn(ptrFn)
    {}

    inline const TVal& Get() const { return m_getFn(); }
    inline TVal* Ptr() { return m_ptrFn(); }

private:
    GetFn<TVal> m_getFn;
    PtrFn<TVal> m_ptrFn;
};