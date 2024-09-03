#pragma once

#include <cstdio>
#include <type_traits>

namespace Paramize {

template<typename type>
concept Opt = std::is_void_v<type>;

template<typename type>
concept Nopt = !std::is_void_v<type>;

template<typename, typename = void>
constexpr bool is_type_complete_v = false;

template<typename type>
constexpr bool is_type_complete_v<type, std::void_t<decltype(sizeof(type))>> = true;

//  FIXME:
template<typename type>
concept AnyTag = !is_type_complete_v<type>;

template<AnyTag tag, AnyTag cmp = void, AnyTag... tags>
constexpr bool is_tag_defined = std::is_same_v<tag, cmp> || is_tag_defined<tag, tags...>;

template<AnyTag tag, AnyTag cmp>
constexpr bool is_tag_defined<tag, cmp> = std::is_same_v<tag, cmp> && !std::is_void_v<cmp>;

template<AnyTag... tags>
struct TagSet {
    template<AnyTag tag>
    static constexpr bool is_part_of = is_tag_defined<tag, tags...>;
};

template<typename type>
constexpr bool is_tag_set = false;

template<AnyTag... tags>
constexpr bool is_tag_set<TagSet<tags...>> = true;

template<typename type>
concept AnyTagSet = is_tag_set<type>;
template<AnyTag tag>
struct ParamValue {
    using _tag = tag;
};

//  FIXME:
template<typename type>
concept AnyParamValue = std::is_base_of_v<ParamValue<typename type::_tag>, type>;

template<typename type>
concept AnyOptParamValue = AnyParamValue<type> || Opt<type>;

template<typename cur = void, Nopt... rest>
constexpr bool is_parameter_list = AnyParamValue<cur> && is_parameter_list<rest...>;

template<Opt cur>
constexpr bool is_parameter_list<cur> = AnyParamValue<cur> || Opt<cur>;
template<AnyParamValue type>
using tagof = type::_tag;

template<AnyParamValue value, AnyTag tag>
constexpr bool has_tag = std::is_same_v<tagof<value>, tag>;

template<AnyOptParamValue, AnyTag>
constexpr bool has_tag_opt = false;

template<AnyParamValue value, AnyTag tag>
constexpr bool has_tag_opt<value, tag> = has_tag<value, tag>;

template<typename type, typename tag>
concept TaggedParamValue = AnyParamValue<type> && AnyTag<tag> && has_tag<type, tag>;

template<AnyParamValue a, AnyParamValue b>
constexpr bool have_same_tag = std::is_same_v<tagof<a>, tagof<b>>;

template<AnyOptParamValue a, AnyOptParamValue b>
constexpr bool have_same_tag_opt = false;

template<AnyParamValue a, AnyParamValue b>
constexpr bool have_same_tag_opt<a, b> = have_same_tag<a, b>;

template<AnyParamValue param, AnyOptParamValue cur = void, AnyParamValue... defined>
constexpr bool is_parameter_unique = !have_same_tag_opt<param, cur> && is_parameter_unique<param, defined...>;

template<AnyParamValue param, AnyOptParamValue cur>
constexpr bool is_parameter_unique<param, cur> = !have_same_tag_opt<param, cur>;

template<typename value, typename set>
concept AnyParam = AnyParamValue<value> && AnyTagSet<set> && set::template is_part_of<tagof<value>>;

template<typename value, typename set>
concept AnyOptParam = AnyParam<value, set> || Opt<value>;

template<typename value, typename set, typename... defined>
concept UniqueParam = AnyParam<value, set> && is_parameter_list<defined...> && is_parameter_unique<value, defined...>;

template<typename value, typename set, typename... defined>
concept OptUniqueParam = UniqueParam<value, set, defined...> || Opt<value>;

template<AnyTag tag, AnyOptParamValue cur = void, AnyOptParamValue... values>
struct tag_value_helper {
    using _value = std::conditional_t<has_tag_opt<cur, tag>, cur, typename tag_value_helper<tag, values...>::_value>;
};

template<AnyTag tag, AnyOptParamValue cur>
struct tag_value_helper<tag, cur> {
    using _value = std::conditional_t<has_tag_opt<cur, tag>, cur, void>;
};

template<AnyTag tag, AnyOptParamValue... values>
using tag_value_opt = tag_value_helper<tag, values...>::_value;

template<AnyTag tag, TaggedParamValue<tag> default_value, AnyOptParamValue... values>
using tag_value = std::conditional_t<!Opt<tag_value_opt<tag, values...>>, tag_value_opt<tag, values...>, default_value>;

}; // namespace Paramize
