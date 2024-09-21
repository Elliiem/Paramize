#pragma once

#include "param_region.hpp"

struct Ok;

template<typename type>
static constexpr bool is_okay_v = false;

template<>
static constexpr bool is_okay_v<Ok> = true;

template<typename type>
concept Okay = is_okay_v<type>;

template<AnyTypeStack results>
struct MergeResultsHelper;

template<AnyFilledTypeStack results>
struct MergeResultsHelper<results> {
    using _cur = results::_top;

    using _next = MergeResultsHelper<typename results::pop>;

    using _value = std::conditional_t<is_okay_v<_cur>, typename _next::_value, _cur>;
};

template<AnyEmptyTypeStack results>
struct MergeResultsHelper<results> {
    using _value = Ok;
};

template<typename... results>
using mergeResults = MergeResultsHelper<TypeStack<results...>>::_value;

// ##################################################

namespace ParseRegionError {
struct TooManyParameters {};
struct TooLittleParameters {};

template<Tag tag>
struct RedifinitionOfUniqueParameter {
    using _tag = tag;
};

template<Tag tag>
struct UnexpectedParameterType {
    using _tag = tag;
};

template<Tag tag>
struct UndefinedRequiredParameter {
    using _tag = tag;
};

}; // namespace ParseRegionError

template<typename type>
static constexpr bool is_parse_region_error_v = false;

template<>
static constexpr bool is_parse_region_error_v<ParseRegionError::TooManyParameters> = true;

template<>
static constexpr bool is_parse_region_error_v<ParseRegionError::TooLittleParameters> = true;

template<Tag tag>
static constexpr bool is_parse_region_error_v<ParseRegionError::RedifinitionOfUniqueParameter<tag>> = true;

template<Tag tag>
static constexpr bool is_parse_region_error_v<ParseRegionError::UnexpectedParameterType<tag>> = true;

template<Tag tag>
static constexpr bool is_parse_region_error_v<ParseRegionError::UndefinedRequiredParameter<tag>> = true;

template<typename type>
concept AnyParseRegionError = is_parse_region_error_v<type>;

template<typename type>
concept AnyCheckRegionResult = Okay<type> || AnyParseRegionError<type>;

template<AnyParseRegionError err, AnyParamRegion region, AnyParamStack params>
struct ParseRegionErrorResult {
    using _err = err;
    using _region = region;
    using _params = params;
};

template<typename type>
static constexpr bool is_parse_region_error_result_v = false;

template<AnyParseRegionError err, AnyParamRegion region, AnyParamStack params>
static constexpr bool is_parse_region_error_result_v<ParseRegionErrorResult<err, region, params>> = true;

template<AnyParamRegion region, AnyTaggedParamStack<typename region::_tagset> params>
struct ParseRegionResult {
    using _region = region;
    using _params = params;
};

template<typename type>
static constexpr bool is_parse_region_result_v = false;

template<AnyParamRegion region, AnyTaggedParamStack<typename region::_tagset> parsed>
static constexpr bool is_parse_region_result_v<ParseRegionResult<region, parsed>> = true;

template<AnyCheckRegionResult check, AnyParamRegion region, AnyParamStack params>
struct ParseRegionResultWrapper;

template<Okay check, AnyParamRegion region, AnyParamStack params>
struct ParseRegionResultWrapper<check, region, params> {
    using _value = ParseRegionResult<region, params>;
};

template<AnyParseRegionError check, AnyParamRegion region, AnyParamStack params>
struct ParseRegionResultWrapper<check, region, params> {
    using _value = ParseRegionErrorResult<check, region, params>;
};

// ########################

template<AnyParamValue param, AnyParamRegion region, AnyParamStack parsed, AnyOptParamRegion next>
struct IsPartOfRegionHelper;

template<AnyParamValue param, AnySetParamRegion region, AnyParamStack parsed, AnyOptParamRegion next>
struct IsPartOfRegionHelper<param, region, parsed, next> {
    static constexpr bool _fits_region = region::_tagset::template contains_v<tagof<param>>;
    static constexpr bool _fits_next_region = check_opt_region_tag_constr_v<next, tagof<param>>;

    static constexpr bool _value = _fits_region || !_fits_next_region;
};

template<AnyParamValue param, AnyListParamRegion region, AnyParamStack parsed, AnyOptParamRegion next>
struct IsPartOfRegionHelper<param, region, parsed, next> {
    static constexpr bool _fits_region = region::_tagset::template contains_v<tagof<param>>;
    static constexpr bool _fits_next_region = check_opt_region_tag_constr_v<next, tagof<param>>;

    static constexpr bool _within_parameter_range = parsed::_lenght + 1 <= region::_max;

    static constexpr bool _value =
        ((_fits_region || !_fits_next_region) && _within_parameter_range) || (!_within_parameter_range && !_fits_next_region);
};

template<AnyParamValue param, AnyParamRegion region, AnyParamStack parsed, AnyOptParamRegion next>
static constexpr bool is_part_of_region_v = IsPartOfRegionHelper<param, region, parsed, next>::_value;

template<AnyParamRegion region, AnyParamStack params, AnyParamStack parsed, AnyOptParamRegion next>
struct GrabRegionHelper {
    using _value = parsed;
};

template<AnyParamRegion region, AnyEmptyParamStack params, AnyParamStack parsed, AnyOptParamRegion next>
struct GrabRegionHelper<region, params, parsed, next> {
    using _value = parsed;
};

template<AnyParamRegion region, AnyFilledParamStack params, AnyParamStack parsed, AnyOptParamRegion next>
    requires is_part_of_region_v<typename params::_top, region, parsed, next>
struct GrabRegionHelper<region, params, parsed, next> {
    using _next_parsed = parsed::template append<typename params::_top>;
    using _next_params = params::pop;

    using _next = GrabRegionHelper<region, _next_params, _next_parsed, next>;

    using _value = _next::_value;
};

template<AnyParamRegion region, AnyParamStack params, AnyOptParamRegion next = Null>
using grabRegion = GrabRegionHelper<region, params, ParamStack<>, next>::_value;

template<AnyParamRegion region, AnyParamStack parsed, AnyParamStack stack, AnyParamStack checked = ParamStack<>>
struct CheckParseRegionResultTypeConstrHelper;

template<AnySetParamRegion region, AnyParamStack parsed, AnyFilledParamStack stack, AnyParamStack checked>
struct CheckParseRegionResultTypeConstrHelper<region, parsed, stack, checked> {
    using _top = stack::_top;
    using _tag = tagof<_top>;

    using _next_stack = stack::pop;
    using _next_checked = checked::template append<_top>;

    static constexpr bool _is_valid_type = region::_tagset::template contains_v<_tag>;
    static constexpr bool _is_redifinition = param_stack_contains_tag_v<checked, _tag>;

    static constexpr bool _is_valid = _is_valid_type && !_is_redifinition;

    using _if_valid = CheckParseRegionResultTypeConstrHelper<region, parsed, _next_stack, _next_checked>::_value;
    // TODO: Find a better way to switch errors
    using _if_not_valid = std::conditional_t<!_is_valid_type, ParseRegionError::UnexpectedParameterType<_tag>,
        std::conditional_t<_is_redifinition, ParseRegionError::RedifinitionOfUniqueParameter<_tag>, Ok>>;

    using _value = std::conditional_t<_is_valid, _if_valid, _if_not_valid>;
};

template<AnySetParamRegion region, AnyParamStack parsed, AnyEmptyParamStack stack, AnyParamStack checked>
struct CheckParseRegionResultTypeConstrHelper<region, parsed, stack, checked> {
    using _value = Ok;
};

template<AnyListParamRegion region, AnyParamStack parsed, AnyFilledParamStack stack, AnyParamStack checked>
struct CheckParseRegionResultTypeConstrHelper<region, parsed, stack, checked> {
    using _cur = stack::_top;
    using _tag = tagof<_cur>;

    using _next_stack = stack::pop;
    using _next_checked = checked::template append<_cur>;

    static constexpr bool _is_valid_type = region::_tagset::template contains_v<_tag>;

    static constexpr bool _is_valid = _is_valid_type;

    using _if_valid = CheckParseRegionResultTypeConstrHelper<region, parsed, _next_stack, _next_checked>::_value;
    using _if_not_valid = std::conditional_t<!_is_valid_type, ParseRegionError::UnexpectedParameterType<_tag>, Ok>;

    using _value = std::conditional_t<_is_valid, _if_valid, _if_not_valid>;
};

template<AnyListParamRegion region, AnyParamStack parsed, AnyEmptyParamStack stack, AnyParamStack checked>
struct CheckParseRegionResultTypeConstrHelper<region, parsed, stack, checked> {
    using _value = Ok;
};

template<AnyParamRegion region, AnyParamStack params>
using checkParseRegionResultTypeConstr = CheckParseRegionResultTypeConstrHelper<region, params, params>::_value;

template<AnyParamRegion region, AnyParamStack params, AnyParamStack defaults, AnyTagStack tags, AnyTagStack checked = TagStack<>>
struct CheckRegionResultDefinitionHelper;

template<AnySetParamRegion region, AnyParamStack params, AnyParamStack defaults, AnyFilledTagStack tags, AnyTagStack checked>
struct CheckRegionResultDefinitionHelper<region, params, defaults, tags, checked> {
    using _cur = tags::_top;

    static constexpr bool _is_defined = param_stack_contains_tag_v<params, _cur>;
    static constexpr bool _has_default = param_stack_contains_tag_v<defaults, _cur>;

    static constexpr bool _is_valid = _is_defined || _has_default;

    using _if_valid = CheckRegionResultDefinitionHelper<region, params, defaults, typename tags::pop, typename checked::template append<_cur>>::_value;
    using _if_not_valid = ParseRegionError::UndefinedRequiredParameter<_cur>;

    using _value = std::conditional_t<_is_valid, _if_valid, _if_not_valid>;
};

template<AnySetParamRegion region, AnyParamStack params, AnyParamStack defaults, AnyEmptyTagStack tags, AnyTagStack checked>
struct CheckRegionResultDefinitionHelper<region, params, defaults, tags, checked> {
    using _value = Ok;
};

template<AnyParamRegion region, AnyParamStack params, AnyParamStack defaults>
using checkParseRegionResultDefinition = CheckRegionResultDefinitionHelper<region, params, defaults, typename region::_tagset>::_value;

template<AnyParamRegion region, AnyParamStack parsed>
struct CheckParseRegionResultHelper;

template<AnySetParamRegion region, AnyParamStack parsed>
struct CheckParseRegionResultHelper<region, parsed> {
    using _type_constr_check = checkParseRegionResultTypeConstr<region, parsed>;
    using _definition_check = checkParseRegionResultDefinition<region, parsed, typename region::_defaults>;

    using _value = mergeResults<_type_constr_check, _definition_check>;
};

template<AnyListParamRegion region, AnyParamStack parsed>
struct CheckParseRegionResultHelper<region, parsed> {
    using _type_constr_check = CheckParseRegionResultTypeConstrHelper<region, parsed, parsed, ParamStack<>>::_value;

    static constexpr bool _has_limit = region::_max != 0;

    static constexpr bool _too_many = parsed::_lenght > region::_max && _has_limit;
    static constexpr bool _too_little = parsed::_lenght < region::_min;

    using _value = std::conditional_t<_too_many, ParseRegionError::TooManyParameters,
        std::conditional_t<_too_little, ParseRegionError::TooLittleParameters, _type_constr_check>>;
};

template<AnyParamRegion region, AnyParamStack parsed>
using checkRegionResult = CheckParseRegionResultHelper<region, parsed>::_value;

template<AnyParamRegion region, AnyParamStack params, AnyOptParamRegion next = Null>
struct ParseRegionHelper {
    using _region_params = grabRegion<region, params, next>;

    using _check = checkRegionResult<region, _region_params>;

    using _value = ParseRegionResultWrapper<_check, region, _region_params>::_value;
};

template<AnyParamRegion region, AnyParamStack params, AnyOptParamRegion next = Null>
using parseRegion = ParseRegionHelper<region, params, next>::_value;
