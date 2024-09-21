#pragma once

#include "type_stack.hpp"

template<typename, typename = void>
constexpr bool is_type_complete_v = false;

template<typename type>
constexpr bool is_type_complete_v<type, std::void_t<decltype(sizeof(type))>> = true;

template<typename type>
static constexpr bool is_tag = !is_type_complete_v<type>;

template<typename type>
concept Tag = is_tag<type>;

template<typename type>
struct IsTag {
    static constexpr bool _value = is_tag<type>;
};

template<Tag... stack>
    requires AnySetTypeStack<TypeStack<stack...>>
using TagStack = TypeStack<stack...>;

template<typename type>
static constexpr bool is_tag_stack = is_set_type_stack_v<type> && check_stack_type_constr_v<IsTag, type>;

template<typename type>
concept AnyTagStack = is_tag_stack<type>;

template<typename type>
concept AnyFilledTagStack = AnyTagStack<type> && is_filled_type_stack_v<type>;

template<typename type>
concept AnyEmptyTagStack = AnyTagStack<type> && is_empty_type_stack_v<type>;

template<Tag tag>
struct Parameter {
    using _tag = tag;
};

template<typename type>
static constexpr bool is_parameter_v = std::is_base_of_v<Parameter<typename type::_tag>, type>;

template<Tag tag>
static constexpr bool is_parameter_v<Parameter<tag>> = true;

template<typename type>
concept AnyParamValue = is_parameter_v<type>;

template<AnyParamValue param>
using tagof = param::_tag;

template<typename type>
struct IsParam {
    static constexpr bool _value = is_parameter_v<type>;
};

template<AnyParamValue... stack>
using ParamStack = TypeStack<stack...>;

template<typename type>
static constexpr bool is_parameter_stack_v = is_type_stack_v<type> && check_stack_type_constr_v<IsParam, type>;

template<typename type>
concept AnyParamStack = is_parameter_stack_v<type>;

template<typename type>
concept AnyFilledParamStack = AnyParamStack<type> && is_filled_type_stack_v<type>;

template<typename type>
concept AnyEmptyParamStack = AnyParamStack<type> && is_empty_type_stack_v<type>;

template<AnyParamStack stack, AnyTagStack constr>
struct CheckParamStackTagConstrHelper;

template<AnyFilledParamStack stack, AnyTagStack constr>
struct CheckParamStackTagConstrHelper<stack, constr> {
    using _next = CheckParamStackTagConstrHelper<typename stack::pop, constr>;

    static constexpr bool _value = constr::template contains_v<tagof<typename stack::_top>> && _next::_value;
};

template<AnyEmptyParamStack stack, AnyTagStack constr>
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
concept AnyEmptyTaggedParamStack = AnyTaggedParamStack<type, constr> && is_empty_type_stack_v<type>;

template<typename type>
concept AnySetParamStack = AnyParamStack<type> && is_set_type_stack_v<type>;

template<typename type>
concept AnyFilledSetParamStack = AnySetParamStack<type> && is_filled_type_stack_v<type>;

template<typename type>
concept AnyEmptySetParamStack = AnySetParamStack<type> && is_empty_type_stack_v<type>;

template<typename type, typename constr>
concept AnyTaggedSetParamStack = AnySetParamStack<type> && AnyTaggedParamStack<type, constr>;

template<typename type, typename constr>
concept AnyFilledTaggedSetParamStack = AnyTaggedSetParamStack<type, constr> && is_filled_type_stack_v<type>;

template<typename type, typename constr>
concept AnyEmptyTaggedSetParamStack = AnyTaggedSetParamStack<type, constr> && is_empty_type_stack_v<type>;

template<AnyParamStack stack, Tag search>
struct ParamStackContainsTagHelper;

template<AnyFilledParamStack stack, Tag search>
struct ParamStackContainsTagHelper<stack, search> {
    using _next = ParamStackContainsTagHelper<typename stack::pop, search>;

    static constexpr bool _value = std::is_same_v<tagof<typename stack::_top>, search> || _next::_value;
};

template<AnyEmptyParamStack stack, Tag search>
struct ParamStackContainsTagHelper<stack, search> {
    static constexpr bool _value = false;
};

template<AnyParamStack stack, Tag search>
static constexpr bool param_stack_contains_tag_v = ParamStackContainsTagHelper<stack, search>::_value;

