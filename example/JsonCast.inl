#include <json/json.h>

namespace Json
{

/////////////////// DESERIALIZATION

template <typename Class,
    typename>
Value serialize(const Class& obj)
{
    return Value(obj);
}

template <typename Class,
    typename, typename>
    Value serialize(const Class& obj)
{
    return serialize_impl(obj);
}

template <typename Class,
    typename>
Value serialize_impl(const Class& obj)
{
    Value value(objectValue);
    meta::doForAllMembers<Class>(
        [&obj, &value](auto& member)
    {
        auto& valueName = value[member.getName()];
        if (member.canGetConstRef()) {
          if ( member.is_enum() ) {
            valueName = serialize(member.as_enum().toString(member.get(obj)));
          } else {
            valueName = serialize(member.get(obj));
          }
        } else if (member.hasGetter()) {
          if ( member.is_enum() ) {
            valueName = serialize(member.as_enum().toString(member.getCopy(obj)));
          } else {
            valueName = serialize(member.getCopy(obj)); // passing copy as const ref, it's okay
          }
        } else {
            throw std::runtime_error("Error: can't deserialize member because it's write only");
        }
    }
    );
    return value;
}

template <typename Class,
    typename, typename>
Value serialize_impl(const Class& obj)
{
    return serialize_basic(obj);
}

template <typename Class>
Value serialize_basic(const Class& obj)
{
    return Value(nullValue);
}

// specialization for std::vector
template <typename T>
Value serialize_basic(const std::vector<T>& obj)
{
    Value value(arrayValue);
    value.resize(obj.size());
    int i = 0;
    for (auto& elem : obj) {
        value[i] = serialize(elem);
        ++i;
    }
    return value;
}

// specialization for std::unodered_map
template <typename K, typename V>
Value serialize_basic(const std::unordered_map<K, V>& obj)
{
    Value value(objectValue);
    for (auto& pair : obj) {
        value[castToString(pair.first)] = serialize(pair.second);
    }
    return value;
}

/////////////////// DESERIALIZATION

template <typename Class>
Class deserialize(const Value& obj)
{
    Class c;
    deserialize(c, obj);
    return c;
}

template <typename Class,
    typename>
void deserialize(Class& obj, const Value& object)
{
    if (object.isObject()) {
        meta::doForAllMembers<Class>(
            [&obj, &object](auto& member)
            {
                auto& objName = object[member.getName()];
                if (!objName.isNull()) {
                    using MemberT = meta::get_member_type<decltype(member)>;
                    if (member.hasSetter()) {
                        member.set(obj, deserialize<MemberT>(objName));
                    } else if (member.canGetRef()) {
                        deserialize(member.getRef(obj), objName);
                    } else {
                        throw std::runtime_error("Error: can't deserialize member because it's read only");
                    }
                }
            }
        );
    } else {
        throw std::runtime_error("Error: can't deserialize from Json::Value to Class.");
    }
}

template <typename Class,
    typename, typename>
void deserialize(Class& obj, const Value& object)
{
    obj = deserialize_basic<Class>(object);
}

template <typename T>
T deserialize_basic(const Value& obj)
{
    return T(); // or maybe throw?
}

// specialization for std::vector
template <typename T>
void deserialize(std::vector<T>& obj, const Value& object)
{
    obj.reserve(object.size()); // vector.resize() works only for default constructible types
    for (auto& elem : object) {
        obj.push_back(deserialize<T>(elem)); // push rvalue
    }
}

// specialization for std::unodered_map
template <typename K, typename V>
void deserialize(std::unordered_map<K, V>& obj, const Value& object)
{
    // keys in Json are always strings
    for (auto& keyStr : object.getMemberNames()) {
        obj.emplace(fromString<K>(keyStr), deserialize<V>(object[keyStr])); // rvalue, oh yeah
    }
}

}