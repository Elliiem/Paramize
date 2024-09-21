#pragma once

#include "base_concepts.hpp"

template<AnyTagStack tagset, AnyTaggedSetParamStack<tagset> defaults = ParamStack<>>
struct SetParamRegion {
    using _tagset = tagset;
    using _defaults = defaults;
};

template<AnyTagStack tagset, size_t min, size_t max>
struct ListParamRegion {
    using _tagset = tagset;

    static constexpr size_t _min = min;

    // NOTE: 0 for no limit
    static constexpr size_t _max = max;
};

template<typename type>
static constexpr bool is_set_parameter_region_v = false;

template<AnyTagStack tagset, AnyTaggedSetParamStack<tagset> defaults>
static constexpr bool is_set_parameter_region_v<SetParamRegion<tagset, defaults>> = true;

template<typename type>
static constexpr bool is_list_parameter_region_v = false;

template<AnyTagStack tagset, size_t min, size_t max>
static constexpr bool is_list_parameter_region_v<ListParamRegion<tagset, min, max>> = true;

template<typename type>
static constexpr bool is_parameter_region_v = is_set_parameter_region_v<type> || is_list_parameter_region_v<type>;

template<typename type>
concept AnyParamRegion = is_parameter_region_v<type>;

template<typename type>
concept AnyOptParamRegion = AnyParamRegion<type> || is_null_v<type>;

template<typename type>
concept AnySetParamRegion = AnyParamRegion<type> && is_set_parameter_region_v<type>;

template<typename type>
concept AnyOptSetParamRegion = AnySetParamRegion<type> || is_null_v<type>;

template<typename type>
concept AnyListParamRegion = AnyParamRegion<type> && is_list_parameter_region_v<type>;

template<typename type>
concept AnyOptListParamRegion = AnyListParamRegion<type> || is_null_v<type>;

template<AnyOptParamRegion region, Tag tag>
static constexpr bool check_opt_region_tag_constr_v = false;

template<AnyParamRegion region, Tag tag>
static constexpr bool check_opt_region_tag_constr_v<region, tag> = region::_tagset::template contains_v<tag>;


