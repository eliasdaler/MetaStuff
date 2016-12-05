/* -----------------------------------------------------------------------------------------------

Member<Class, T> is a representation of a registered member
Class - a class this member belongs to
T - type of that member

Member contains pointer to data member which can be used to get / set stuff
Different getters and setters can be added via fluent interface (see addGetter/addConstGetter/addSetter)
Getters and setters are always preffered to using raw data member pointers (because why would you
set them in the other place?)

-------------------------------------------------------------------------------------------------*/

#pragma once

#include <unordered_map>
#include <string>
#include <type_traits>


namespace meta
{

template <typename Class, typename T>
using member_ptr_t = T Class::*;

// reference getter/setter func pointer type
template <typename Class, typename T>
using ref_getter_func_ptr_t = const T& (Class::*)() const;

template <typename Class, typename T>
using ref_setter_func_ptr_t = void (Class::*)(const T&);

// value getter/setter func pointer type
template <typename Class, typename T>
using val_getter_func_ptr_t = T(Class::*)() const;

template <typename Class, typename T>
using val_setter_func_ptr_t = void (Class::*)(T);

// non const reference getter
template <typename Class, typename T>
using nonconst_ref_getter_func_ptr_t = T& (Class::*)();

// MemberType is Member<T, Class>
template <typename MemberType>
using get_member_type = typename std::decay_t<MemberType>::member_type;

template <typename Class, typename T>
class EnumMember;

template <typename Class, typename T>
class Member {
public:
    using class_type = Class;
    using member_type = T;

    Member(const char* name, member_ptr_t<Class, T> ptr);
    Member(const char* name, ref_getter_func_ptr_t<Class, T> getterPtr, ref_setter_func_ptr_t<Class, T> setterPtr);
    Member(const char* name, val_getter_func_ptr_t<Class, T> getterPtr, val_setter_func_ptr_t<Class, T> setterPtr);

    Member& addNonConstGetter(nonconst_ref_getter_func_ptr_t<Class, T> nonConstRefGetterPtr);

    // get sets methods can be used to add support
    // for getters/setters for members instead of
    // direct access to them
    const T& get(const Class& obj) const;
    T getCopy(const Class& obj) const;
    T& getRef(Class& obj) const;
    member_ptr_t<Class, T> getPtr() const;

    template <typename V,
        typename = std::enable_if_t<std::is_constructible<T, V>::value>>
        void set(Class& obj, V&& value) const; // accepts lvalues and rvalues!

    const char* getName() const { return name; }
    bool hasPtr() const { return hasMemberPtr; }
    bool hasGetter() const { return refGetterPtr || valGetterPtr; }
    bool hasSetter() const { return refSetterPtr || valSetterPtr; }
    bool canGetConstRef() const { return hasMemberPtr || refGetterPtr; }
    bool canGetRef() const { return hasMemberPtr || nonConstRefGetterPtr; }
    bool is_enum() const { return std::is_enum<T>::value; }
    
    // Downcast to EnumMemberType, should only use if is_enum is true.
    using EnumMemberType = EnumMember<Class, T>;
    const EnumMemberType as_enum() const {
      assert(this->is_enum() && "to_enum_member used for non-enum type!");
      const EnumMemberType& enum_member = static_cast<const EnumMemberType&>(*this);
      return enum_member; 
    }
private:
    const char* name;
    member_ptr_t<Class, T> ptr;
    bool hasMemberPtr; // first member of class can be nullptr
                       // so we need this var to know if member ptr is present

    ref_getter_func_ptr_t<Class, T> refGetterPtr;
    ref_setter_func_ptr_t<Class, T> refSetterPtr;

    val_getter_func_ptr_t<Class, T> valGetterPtr;
    val_setter_func_ptr_t<Class, T> valSetterPtr;

    nonconst_ref_getter_func_ptr_t<Class, T> nonConstRefGetterPtr;
};


template<typename T>
struct EnumClassHash {
  template<typename U = T,
    typename = std::enable_if_t<std::is_enum<T>::value>>
  std::size_t operator()(T const& val) const {
    using underlying_type = typename std::underlying_type<T>::type;
    const underlying_type u_val = static_cast<underlying_type>(val);
    std::hash<underlying_type> hfn;
    return hfn(u_val);
  }

  std::size_t operator()(T const& val) const {
    assert("Something has gone horribly wrong for you to call this on a non-enum type");
    return 0;
  }
};

template <typename, typename>
class EnumVoidMap {
public:

};


template <typename Class, typename T>
class EnumMember : public Member<Class, T> {
public:
    using Member<Class, T>::Member;

    // Used for when Member is of type Enum
    using to_str_map_type = std::unordered_map<T, std::string, EnumClassHash<T>>;
    using from_str_map_type = std::unordered_map<std::string, T>;

    // Enable assigning .value() for enum members
    template <typename U = T>
    typename std::enable_if<std::is_enum<U>::value, EnumMember&>::type
    value(const char* name, T value) {
      auto& to_str_map = this->to_str_map();
      auto& from_str_map = this->from_str_map();
      to_str_map[value] = name;
      from_str_map[name] = value;
      return *this;
    }


    template <typename U = T>
    typename std::enable_if<std::is_enum<U>::value, std::string&>::type
    toString(const T value) const {
      auto& to_str_map = this->to_str_map();
      return to_str_map[value];
    }

    template <typename U = T>
    typename std::enable_if<std::is_enum<U>::value, U>::type
    fromString(const std::string& name) const {
      auto& from_str_map = this->from_str_map();
      return from_str_map[name];
    }

    template <typename U = T>
    typename std::enable_if<!std::is_enum<U>::value, std::string>::type
    toString(const T value) const {
      assert("Called EnumMember method on non-enum type class. Something went horribly wrong!");
      (void)value;
      return std::string("");;
    }

    template <typename U = T>
    typename std::enable_if<!std::is_enum<U>::value, U>::type
    fromString(const std::string& name) const {
      assert("Called EnumMember method on non-enum type class. Something went horribly wrong!");
      (void)name;
      U v;
      return v;
    }
private:
  // Provide static maps for enums
  template <typename U = T>
  typename std::enable_if<!std::is_enum<U>::value, EnumVoidMap<U,U>>::type
    to_str_map() const {
    assert("Called EnumMember method on non-enum type class. Something went horribly wrong!");
    return EnumVoidMap();
  }

  template <typename U = T>
  typename std::enable_if<!std::is_enum<U>::value, EnumVoidMap<U, U>>::type
    from_str_map() const {
    assert("Called EnumMember method on non-enum type class. Something went horribly wrong!");
    return EnumVoidMap();
  }

  // Enable assigning .value() for enum members
  template <typename U = T>
  typename std::enable_if<!std::is_enum<U>::value, EnumMember&>::type
    value(const char* name, T value) {
    assert("Called EnumMember method on non-enum type class. Something went horribly wrong!");
    return *this;
  }

  // Provide static maps for enums
  template <typename U = T>
  typename std::enable_if<std::is_enum<U>::value, to_str_map_type&>::type
    to_str_map() const {
    static to_str_map_type map;
    return map;
  }

  template <typename U = T>
  typename std::enable_if<std::is_enum<U>::value, from_str_map_type&>::type
    from_str_map() const {
    static from_str_map_type map;
    return map;
  }
};

// useful function similar to make_pair which is used so you don't have to write this:
// Member<SomeClass, int>("someName", &SomeClass::someInt); and can just to this:
// member("someName", &SomeClass::someInt);

template <typename Class, typename T>
Member<Class, T> member(const char* name, T Class::* ptr);

template <typename Class, typename T>
Member<Class, T> member(const char* name, ref_getter_func_ptr_t<Class, T> getterPtr, ref_setter_func_ptr_t<Class, T> setterPtr);

template <typename Class, typename T>
Member<Class, T> member(const char* name, val_getter_func_ptr_t<Class, T> getterPtr, val_setter_func_ptr_t<Class, T> setterPtr);

// Useful for assigning enums
// member("someName", &SomeClass::someEnum)
//  .value("VAL1", SomeEnum::Val1);
template <typename Class, typename T,
   typename = std::enable_if_t<std::is_enum<T>::value>>
  EnumMember<Class, T> enummember(const char* name, T Class::* ptr);

template <typename Class, typename T,
  typename = std::enable_if_t<std::is_enum<T>::value>>
EnumMember<Class, T> enummember(const char* name, ref_getter_func_ptr_t<Class, T> getterPtr, ref_setter_func_ptr_t<Class, T> setterPtr);

template <typename Class, typename T,
  typename = std::enable_if_t<std::is_enum<T>::value>>
EnumMember<Class, T> enummember(const char* name, val_getter_func_ptr_t<Class, T> getterPtr, val_setter_func_ptr_t<Class, T> setterPtr);

} // end of namespace meta

#include "Member.inl"
