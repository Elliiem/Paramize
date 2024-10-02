#pragma once

#include <cstddef>
#include <type_traits>

#include "tag.hpp"

#include "tag_stack.hpp"
// #include "type_stack.hpp"

template<typename type>
struct IsParameterTag {
    static constexpr bool _value = is_parameter_tag_v<type>;
};

template<AnyTagStack stack>
static constexpr bool is_typed_tag_stack_v<stack, ParameterTag> = check_stack_type_constr_v<IsParameterTag, stack>;

template<typename type>
struct IsRegionTag {
    static constexpr bool _value = is_region_tag_v<type>;
};

template<AnyTagStack stack>
static constexpr bool is_typed_tag_stack_v<stack, RegionTag> = check_stack_type_constr_v<IsRegionTag, stack>;

template<typename type, typename tag_type>
concept TypedTagStack = AnyTagStack<type> && is_typed_tag_stack_v<type, tag_type>;

template<typename type>
concept AnyParamTagStack = TypedTagStack<type, ParameterTag>;

template<typename type>
concept AnyRegionTagStack = TypedTagStack<type, RegionTag>;

template<typename tag, typename tagset>
concept MemberTag = AnyTag<tag> && TypedTagStack<tagset, getTagType<tag>> && tagset::template contains<tag>;

template<AnyParamTag tag>
struct Parameter {
    using _tag = tag;
};

template<typename type>
static constexpr bool is_parameter_v = std::is_base_of_v<Parameter<typename type::_tag>, type>;

template<AnyParamTag tag>
static constexpr bool is_parameter_v<Parameter<tag>> = true;

template<typename type>
concept AnyParamValue = is_parameter_v<type>;

template<AnyParamValue param>
using tagof = param::_tag;

template<typename type>
struct IsParameter {
    static constexpr bool _value = is_parameter_v<type>;
};

template<AnyParamValue... stack>
using ParamStack = TypeStack<stack...>;

template<typename type>
static constexpr bool is_parameter_stack_v = is_type_stack_v<type> && check_stack_type_constr_v<IsParameter, type>;

template<typename type>
concept AnyParamStack = is_parameter_stack_v<type>;

template<typename type>
concept AnyFilledParamStack = AnyParamStack<type> && is_filled_type_stack_v<type>;

template<typename type>
concept EmptyParamStack = AnyParamStack<type> && is_empty_type_stack_v<type>;

template<AnyParamStack stack, AnyTagStack constr>
struct CheckParamStackTagConstrHelper;

template<AnyFilledParamStack stack, AnyTagStack constr>
struct CheckParamStackTagConstrHelper<stack, constr> {
    using _next = CheckParamStackTagConstrHelper<typename stack::template pop<1>, constr>;

    static constexpr bool _value = constr::template contains<tagof<typename stack::_top>> && _next::_value;
};

template<EmptyParamStack stack, AnyTagStack constr>
struct CheckParamStackTagConstrHelper<stack, constr> {
    static constexpr bool _value = true;
};

template<AnyParamStack stack, AnyTagStack constr>
static constexpr bool check_param_stack_tag_constr_v = CheckParamStackTagConstrHelper<stack, constr>::_value;

template<typename type, typename constr>
concept AnyTaggedParamStack = AnyParamStack<type> && AnyTagStack<constr> && check_param_stack_tag_constr_v<type, constr>;

template<typename type, typename constr>
concept AnyFilledTaggedParamStack = AnyTaggedParamStack<type, constr> && is_filled_type_stack_v<type>;

template<typename type, typename constr>
concept EmptyTaggedParamStack = AnyTaggedParamStack<type, constr> && is_empty_type_stack_v<type>;

template<typename type>
concept AnySetParamStack = AnyParamStack<type> && is_set_type_stack_v<type>;

template<typename type>
concept AnyFilledSetParamStack = AnySetParamStack<type> && is_filled_type_stack_v<type>;

template<typename type>
concept EmptySetParamStack = AnySetParamStack<type> && is_empty_type_stack_v<type>;

template<typename type, typename constr>
concept AnyTaggedSetParamStack = AnySetParamStack<type> && AnyTaggedParamStack<type, constr>;

template<typename type, typename constr>
concept AnyFilledTaggedSetParamStack = AnyTaggedSetParamStack<type, constr> && is_filled_type_stack_v<type>;

template<typename type, typename constr>
concept EmptyTaggedSetParamStack = AnyTaggedSetParamStack<type, constr> && is_empty_type_stack_v<type>;

template<AnyParamStack stack, AnyParamTag tag, size_t i = 0>
struct FindParamHelper {
    using _next = FindParamHelper<typename stack::template pop<1>, tag, i + 1>;

    static constexpr size_t _value = std::is_same_v<tagof<typename stack::_top>, tag> ? i : _next::_value;
};

template<EmptyParamStack stack, AnyParamTag tag, size_t i>
struct FindParamHelper<stack, tag, i> {
    static constexpr size_t _value = -1;
};

template<AnyParamStack stack, AnyParamTag tag, size_t start = 0>
static constexpr size_t findParam = FindParamHelper<stack, tag, start>::_value;

template<AnyParamStack stack, AnyParamTag search>
struct ParamStackContainsTagHelper;

template<AnyFilledParamStack stack, AnyParamTag search>
struct ParamStackContainsTagHelper<stack, search> {
    using _next = ParamStackContainsTagHelper<typename stack::template pop<1>, search>;

    static constexpr bool _value = std::is_same_v<tagof<typename stack::_top>, search> || _next::_value;
};

template<EmptyParamStack stack, AnyParamTag search>
struct ParamStackContainsTagHelper<stack, search> {
    static constexpr bool _value = false;
};

template<AnyParamStack stack, AnyParamTag search>
static constexpr bool param_stack_contains_tag_v = ParamStackContainsTagHelper<stack, search>::_value;

template<AnyParamValue param, AnyParamTag cmp>
struct SameTagCmp {
    static constexpr bool _value = std::is_same_v<tagof<param>, cmp>;
};
