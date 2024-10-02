#pragma once

#include "result.hpp"
//  #include "type_stack.hpp"

#include "parameter.hpp"
//  #include "tag.hpp"
//      #include "type_stack.hpp"

// ##################################################

template<AnyRegionTag tag, AnyParamTagStack tagset, AnyTaggedSetParamStack<tagset> defaults = ParamStack<>>
struct SetParamRegion {
    using _tag = tag;
    using _tagset = tagset;
    using _defaults = defaults;
};

template<AnyRegionTag tag, AnyParamTagStack tagset, size_t min, size_t max>
struct ListParamRegion {
    using _tag = tag;
    using _tagset = tagset;

    static constexpr size_t _min = min;

    //  NOTE: 0 for no limit
    static constexpr size_t _max = max;
};

template<typename type>
static constexpr bool is_set_parameter_region_v = false;

template<AnyRegionTag tag, AnyTagStack tagset, AnyTaggedSetParamStack<tagset> defaults>
static constexpr bool is_set_parameter_region_v<SetParamRegion<tag, tagset, defaults>> = true;

template<typename type>
static constexpr bool is_list_parameter_region_v = false;

template<AnyRegionTag tag, AnyParamTagStack tagset, size_t min, size_t max>
static constexpr bool is_list_parameter_region_v<ListParamRegion<tag, tagset, min, max>> = true;

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

template<AnyOptParamRegion region, AnyParamTag tag>
static constexpr bool check_opt_region_tag_constr_v = false;

template<AnyParamRegion region, AnyParamTag tag>
static constexpr bool check_opt_region_tag_constr_v<region, tag> = region::_tagset::template contains<tag>;

template<AnyParamRegion... regions>
using ParamRegionStack = TypeStack<regions...>;

template<typename type>
struct IsParamRegion {
    static constexpr bool _value = is_parameter_region_v<type>;
};

template<typename type>
static constexpr bool is_parameter_region_stack_v = is_type_stack_v<type> && check_stack_type_constr_v<IsParamRegion, type>;

template<typename type>
concept AnyParamRegionStack = is_parameter_region_stack_v<type>;

template<typename type>
concept AnyFilledParamRegionStack = AnyParamRegionStack<type> && is_filled_type_stack_v<type>;

template<typename type>
concept EmptyParamRegionStack = AnyParamRegionStack<type> && is_empty_type_stack_v<type>;

// #################################################
// # Parser                                        #
// #################################################

namespace ParseRegionError {
struct TooManyParameters {};
struct TooLittleParameters {};

template<AnyParamTag tag>
struct RedifinitionOfUniqueParameter {
    using _tag = tag;
};

template<AnyParamTag tag>
struct UnexpectedParameterType {
    using _tag = tag;
};

template<AnyParamTag tag>
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

template<AnyParamTag tag>
static constexpr bool is_parse_region_error_v<ParseRegionError::RedifinitionOfUniqueParameter<tag>> = true;

template<AnyParamTag tag>
static constexpr bool is_parse_region_error_v<ParseRegionError::UnexpectedParameterType<tag>> = true;

template<AnyParamTag tag>
static constexpr bool is_parse_region_error_v<ParseRegionError::UndefinedRequiredParameter<tag>> = true;

template<typename type>
concept AnyParseRegionError = is_parse_region_error_v<type>;

template<typename type>
concept AnyParseRegionRawResult = Okay<type> || AnyParseRegionError<type>;

// - - - - - - - - - - - - - - - - - - - - - - - - -

template<AnyParamRegion region, AnyTaggedParamStack<typename region::_tagset> params>
struct ParseRegionOkayResult {
    using _region = region;
    using _params = params;
};

template<typename type>
static constexpr bool is_parse_region_okay_result_v = false;

template<AnyParamRegion region, AnyTaggedParamStack<typename region::_tagset> parsed>
static constexpr bool is_parse_region_okay_result_v<ParseRegionOkayResult<region, parsed>> = true;

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

template<typename type>
static constexpr bool is_parse_region_result_v = is_parse_region_okay_result_v<type> || is_parse_region_error_result_v<type>;

template<typename type>
concept AnyParseRegionResult = is_parse_region_result_v<type>;

template<typename type>
concept AnyParseRegionErrorResult = AnyParseRegionResult<type> && is_parse_region_error_result_v<type>;

template<typename type>
concept AnyParseRegionOkayResult = AnyParseRegionResult<type> && is_parse_region_okay_result_v<type>;

// - - - - - - - - - - - - - - - - - - - - - - - - -

template<AnyParseRegionResult... stack>
using ParseRegionResultStack = TypeStack<stack...>;

template<typename type>
struct IsParseRegionResult {
    static constexpr bool _value = is_parse_region_result_v<type>;
};

template<typename type>
static constexpr bool is_parse_region_result_stack_v = is_type_stack_v<type> && check_stack_type_constr_v<IsParseRegionResult, type>;

template<typename type>
concept AnyParseRegionResultStack = is_parse_region_result_stack_v<type>;

template<typename type>
concept AnyFilledParseRegionResultStack = AnyParseRegionResultStack<type> && is_filled_type_stack_v<type>;

template<typename type>
concept EmptyParseRegionResultStack = AnyParseRegionResultStack<type> && is_empty_type_stack_v<type>;

template<AnyParseRegionRawResult check, AnyParamRegion region, AnyParamStack params>
struct ParseRegionResultWrapper;

template<Okay check, AnyParamRegion region, AnyParamStack params>
struct ParseRegionResultWrapper<check, region, params> {
    using _value = ParseRegionOkayResult<region, params>;
};

template<AnyParseRegionError check, AnyParamRegion region, AnyParamStack params>
struct ParseRegionResultWrapper<check, region, params> {
    using _value = ParseRegionErrorResult<check, region, params>;
};

// - - - - - - - - - - - - - - - - - - - - - - - - -

template<AnyParamValue param, AnyParamRegion region, AnyParamStack parsed, AnyOptParamRegion next>
struct IsPartOfRegionHelper;

template<AnyParamValue param, AnySetParamRegion region, AnyParamStack parsed, AnyOptParamRegion next>
struct IsPartOfRegionHelper<param, region, parsed, next> {
    static constexpr bool _fits_region = region::_tagset::template contains<tagof<param>>;
    static constexpr bool _fits_next_region = check_opt_region_tag_constr_v<next, tagof<param>>;

    static constexpr bool _value = _fits_region || !_fits_next_region;
};

template<AnyParamValue param, AnyListParamRegion region, AnyParamStack parsed, AnyOptParamRegion next>
struct IsPartOfRegionHelper<param, region, parsed, next> {
    static constexpr bool _fits_region = region::_tagset::template contains<tagof<param>>;
    static constexpr bool _fits_next_region = check_opt_region_tag_constr_v<next, tagof<param>>;

    static constexpr bool _within_parameter_range = parsed::_lenght + 1 <= region::_max;

    static constexpr bool _value =
        ((_fits_region || !_fits_next_region) && _within_parameter_range) || (!_within_parameter_range && !_fits_next_region);
};

template<AnyParamValue param, AnyParamRegion region, AnyParamStack parsed, AnyOptParamRegion next>
static constexpr bool is_part_of_region_v = IsPartOfRegionHelper<param, region, parsed, next>::_value;

// - - - - - - - - - - - - - - - - - - - - - - - - -

template<AnyParamRegion region, AnyParamStack params, AnyParamStack parsed, AnyOptParamRegion next>
struct GrabRegionHelper {
    using _value = parsed;
};

template<AnyParamRegion region, EmptyParamStack params, AnyParamStack parsed, AnyOptParamRegion next>
struct GrabRegionHelper<region, params, parsed, next> {
    using _value = parsed;
};

template<AnyParamRegion region, AnyFilledParamStack params, AnyParamStack parsed, AnyOptParamRegion next>
    requires is_part_of_region_v<typename params::_top, region, parsed, next>
struct GrabRegionHelper<region, params, parsed, next> {
    using _next_parsed = parsed::template push<typename params::_top>;
    using _next_params = params::template pop<1>;

    using _next = GrabRegionHelper<region, _next_params, _next_parsed, next>;

    using _value = _next::_value;
};

template<AnyParamRegion region, AnyParamStack params, AnyOptParamRegion next = Null>
using grabRegion = GrabRegionHelper<region, params, ParamStack<>, next>::_value;

// - - - - - - - - - - - - - - - - - - - - - - - - -

template<AnyParamRegion region, AnyParamStack parsed, AnyParamStack stack, AnyParamStack checked = ParamStack<>>
struct CheckParseRegionResultTypeConstrHelper;

template<AnySetParamRegion region, AnyParamStack parsed, AnyFilledParamStack stack, AnyParamStack checked>
struct CheckParseRegionResultTypeConstrHelper<region, parsed, stack, checked> {
    using _top = stack::_top;
    using _tag = tagof<_top>;

    using _next_stack = stack::template pop<1>;
    using _next_checked = checked::template push<_top>;

    static constexpr bool _is_valid_type = region::_tagset::template contains<_tag>;
    static constexpr bool _is_redifinition = checked::template contains<_tag, SameTagCmp>;

    static constexpr bool _is_valid = _is_valid_type && !_is_redifinition;

    using _if_valid = CheckParseRegionResultTypeConstrHelper<region, parsed, _next_stack, _next_checked>::_value;
    // TODO: Find a better way to switch errors
    using _if_not_valid = std::conditional_t<!_is_valid_type, ParseRegionError::UnexpectedParameterType<_tag>,
        std::conditional_t<_is_redifinition, ParseRegionError::RedifinitionOfUniqueParameter<_tag>, Ok>>;

    using _value = std::conditional_t<_is_valid, _if_valid, _if_not_valid>;
};

template<AnySetParamRegion region, AnyParamStack parsed, EmptyParamStack stack, AnyParamStack checked>
struct CheckParseRegionResultTypeConstrHelper<region, parsed, stack, checked> {
    using _value = Ok;
};

template<AnyListParamRegion region, AnyParamStack parsed, AnyFilledParamStack stack, AnyParamStack checked>
struct CheckParseRegionResultTypeConstrHelper<region, parsed, stack, checked> {
    using _cur = stack::_top;
    using _tag = tagof<_cur>;

    using _next_stack = stack::template pop<1>;
    using _next_checked = checked::template push<_cur>;

    static constexpr bool _is_valid_type = region::_tagset::template contains<_tag>;

    static constexpr bool _is_valid = _is_valid_type;

    using _if_valid = CheckParseRegionResultTypeConstrHelper<region, parsed, _next_stack, _next_checked>::_value;
    using _if_not_valid = std::conditional_t<!_is_valid_type, ParseRegionError::UnexpectedParameterType<_tag>, Ok>;

    using _value = std::conditional_t<_is_valid, _if_valid, _if_not_valid>;
};

template<AnyListParamRegion region, AnyParamStack parsed, EmptyParamStack stack, AnyParamStack checked>
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

    static constexpr bool _is_defined = params::template contains<_cur, SameTagCmp>;
    static constexpr bool _has_default = defaults::template contains<_cur, SameTagCmp>;

    static constexpr bool _is_valid = _is_defined || _has_default;

    using _if_valid =
        CheckRegionResultDefinitionHelper<region, params, defaults, typename tags::template pop<1>, typename checked::template push<_cur>>::_value;
    using _if_not_valid = ParseRegionError::UndefinedRequiredParameter<_cur>;

    using _value = std::conditional_t<_is_valid, _if_valid, _if_not_valid>;
};

template<AnySetParamRegion region, AnyParamStack params, AnyParamStack defaults, EmptyTagStack tags, AnyTagStack checked>
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

    static constexpr bool _too_many = parsed::_lenght > region::_max&& _has_limit;
    static constexpr bool _too_little = parsed::_lenght < region::_min;

    using _value = std::conditional_t<_too_many, ParseRegionError::TooManyParameters,
        std::conditional_t<_too_little, ParseRegionError::TooLittleParameters, _type_constr_check>>;
};

template<AnyParamRegion region, AnyParamStack parsed>
using checkRegionResult = CheckParseRegionResultHelper<region, parsed>::_value;

// - - - - - - - - - - - - - - - - - - - - - - - - -

template<AnyParamRegion region, AnyParamStack params, AnyOptParamRegion next = Null>
struct ParseRegionHelper {
    using _region_params = grabRegion<region, params, next>;

    using _check = checkRegionResult<region, _region_params>;

    using _value = ParseRegionResultWrapper<_check, region, _region_params>::_value;
};

template<AnyParamRegion region, AnyParamStack params, AnyOptParamRegion next = Null>
using parseRegion = ParseRegionHelper<region, params, next>::_value;

template<AnyParseRegionResultStack parsed>
struct AreParseRegionResultsValidHelper;

template<AnyFilledParseRegionResultStack parsed>
struct AreParseRegionResultsValidHelper<parsed> {
    using _cur = parsed::_top;

    using _next = AreParseRegionResultsValidHelper<typename parsed::template pop<1>>;

    static constexpr bool _value = is_parse_region_okay_result_v<_cur> && _next::_value;
};

template<EmptyParseRegionResultStack parsed>
struct AreParseRegionResultsValidHelper<parsed> {
    static constexpr bool _value = true;
};

template<AnyParseRegionResultStack parsed>
static constexpr bool are_parse_region_results_valid_v = AreParseRegionResultsValidHelper<parsed>::_value;

// - - - - - - - - - - - - - - - - - - - - - - - - -

namespace ParseParamsError {
struct TrailingParameters;
}

template<typename type>
static constexpr bool is_parse_parameters_error = false;

template<>
static constexpr bool is_parse_parameters_error<ParseParamsError::TrailingParameters> = false;

template<typename type>
concept AnyParseParamsError = is_parse_parameters_error<type>;

template<typename type>
concept AnyParseParamsRawResult = Okay<type> || AnyParseParamsError<type>;

template<AnyParseRegionResultStack parsed>
struct ParseParamsOkayResult {
    using _parsed = parsed;
};

template<typename type>
static constexpr bool is_parse_parameters_okay_result = false;

template<AnyParseRegionResultStack parsed>
static constexpr bool is_parse_parameters_okay_result<ParseParamsOkayResult<parsed>> = true;

template<AnyParseParamsError err, AnyParseRegionResultStack parsed, AnyParamStack trailing>
struct ParseParamsErrorResult {
    using _err = err;

    using _trailing = trailing;

    using _parsed = parsed;
};

template<typename type>
static constexpr bool is_parse_parameters_error_result = false;

template<AnyParseParamsError result, AnyParseRegionResultStack parsed, AnyParamStack trailing>
static constexpr bool is_parse_parameters_error_result<ParseParamsErrorResult<result, parsed, trailing>> = true;

template<typename type>
concept AnyParseParamsResult = is_parse_parameters_okay_result<type> || is_parse_parameters_error_result<type>;

template<AnyParseParamsRawResult result, AnyParseRegionResultStack parsed, AnyParamStack trailing>
struct ParseParamsResultWrapper;

template<Okay result, AnyParseRegionResultStack parsed, AnyParamStack trailing>
struct ParseParamsResultWrapper<result, parsed, trailing> {
    using _value = ParseParamsOkayResult<parsed>;
};

template<AnyParseParamsError result, AnyParseRegionResultStack parsed, AnyParamStack trailing>
struct ParseParamsResultWrapper<result, parsed, trailing> {
    using _value = ParseParamsErrorResult<result, parsed, trailing>;
};

// - - - - - - - - - - - - - - - - - - - - - - - - -

template<AnyParamRegionStack regions, AnyParamStack params, AnyParseRegionResultStack parsed = ParseRegionResultStack<>>
struct ParseParametersHelper;

template<AnyFilledParamRegionStack regions, AnyParamStack params, AnyParseRegionResultStack parsed>
struct ParseParametersHelper<regions, params, parsed> {
    using _cur = regions::_top;
    using _next_region = regions::template pop<1>::_top;

    using _parsed_region = parseRegion<_cur, params, _next_region>;

    using _value = ParseParametersHelper<typename regions::template pop<1>, typename params::template pop<_parsed_region::_params::_lenght>,
        typename parsed::template push<_parsed_region>>::_value;
};

template<EmptyParamRegionStack regions, AnyParamStack params, AnyParseRegionResultStack parsed>
struct ParseParametersHelper<regions, params, parsed> {
    using _result = std::conditional_t<params::_lenght == 0, Ok, ParseParamsError::TrailingParameters>;

    using _value = ParseParamsResultWrapper<_result, parsed, params>::_value;
};

// #################################################
template<AnyParamRegionStack regions, AnyParamStack params>
using parseParameters = ParseParametersHelper<regions, params>::_value;
// #################################################

template<AnyParseParamsResult parsed>
struct AreValidParametersHelper {
    static constexpr bool _is_error = is_parse_parameters_error_result<parsed>;

    static constexpr bool _are_regions_perfect = are_parse_region_results_valid_v<typename parsed::_parsed>;

    static constexpr bool _value = !_is_error && _are_regions_perfect;
};

// #################################################
template<typename regions, typename... params>
concept ValidParameters = is_parameter_region_stack_v<regions> && is_parameter_stack_v<TypeStack<params...>> &&
                          AreValidParametersHelper<parseParameters<regions, ParamStack<params...>>>::_value;
// #################################################

