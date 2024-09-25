#pragma once

#include <type_traits>

struct ParameterTag {};
struct RegionTag {};

template<typename type>
static constexpr bool is_tag_type_v = false;

template<>
static constexpr bool is_tag_type_v<ParameterTag> = true;

template<>
static constexpr bool is_tag_type_v<RegionTag> = true;

template<typename type>
concept AnyTagType = is_tag_type_v<type>;

template<typename type>
static constexpr bool is_parameter_tag_v = std::is_base_of_v<ParameterTag, type>;

template<>
static constexpr bool is_parameter_tag_v<ParameterTag> = true;

template<typename type>
static constexpr bool is_region_tag_v = std::is_base_of_v<RegionTag, type>;

template<>
static constexpr bool is_region_tag_v<RegionTag> = true;

template<typename type>
static constexpr bool is_tag = is_parameter_tag_v<type> || is_region_tag_v<type>;

template<typename type>
concept AnyTag = is_tag<type>;

template<typename type>
concept AnyParamTag = AnyTag<type> && is_parameter_tag_v<type>;

template<typename type>
concept AnyRegionTag = AnyTag<type> && is_region_tag_v<type>;

template<AnyTag tag>
struct GetTagTypeHelper;

template<AnyParamTag tag>
struct GetTagTypeHelper<tag> {
    using _value = ParameterTag;
};

template<AnyRegionTag tag>
struct GetTagTypeHelper<tag> {
    using _value = RegionTag;
};

template<AnyTag tag>
using getTagType = GetTagTypeHelper<tag>::_value;

