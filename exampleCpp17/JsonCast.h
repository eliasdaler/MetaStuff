#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <variant>

#include <json.hpp>

#include <Meta.h>
#include "StringCast.h"

template <typename T>
void to_json(nlohmann::json& j, const T& obj);

template <typename T>
void from_json(const nlohmann::json& j, T& obj);

namespace meta
{

/////////////////// SERIALIZATION

template <typename Class,
    typename = std::enable_if_t <meta::isRegistered<Class>()>>
nlohmann::json serialize(const Class& obj);

template <typename Class,
    typename = std::enable_if_t <!meta::isRegistered<Class>()>,
    typename = void>
nlohmann::json serialize(const Class& obj);

template <typename Class>
nlohmann::json serialize_basic(const Class& obj);

// specialization for std::vector
template <typename T>
nlohmann::json serialize_basic(const std::vector<T>& obj);

// specialization for std::unodered_map
template <typename K, typename V>
nlohmann::json serialize_basic(const std::unordered_map<K, V>& obj);

// specialization for std::variant
template <typename... T>
nlohmann::json serialize_basic(const std::variant<T...>& obj);
inline
nlohmann::json serialize_basic(const std::monostate& obj);

/////////////////// DESERIALIZATION
//

template <typename Class,
    typename = std::enable_if_t<meta::isRegistered<Class>()>>
void deserialize(Class& obj, const nlohmann::json& object);

template <typename Class,
    typename = std::enable_if_t<!meta::isRegistered<Class>()>,
    typename = void>
void deserialize(Class& obj, const nlohmann::json& object);

template <typename Class>
void deserialize_basic(Class& obj, const nlohmann::json& object);

// specialization for std::vector
template <typename T>
void deserialize_basic(std::vector<T>& obj, const nlohmann::json& object);

// specialization for std::unodered_map
template <typename K, typename V>
void deserialize_basic(std::unordered_map<K, V>& obj, const nlohmann::json& object);

// specialization for std::variant
template <typename... T>
void deserialize_basic(std::variant<T...>& ret, const nlohmann::json& object);
inline
void deserialize_basic(std::monostate& ret, const nlohmann::json& object);

} // namespace meta

#include "JsonCast.inl"