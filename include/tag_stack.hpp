#pragma once

#include "tag.hpp"
#include "type_stack.hpp"

template<typename type>
struct IsTag {
    static constexpr bool _value = is_tag<type>;
};

template<AnyParamTag... stack>
    requires AnySetTypeStack<TypeStack<stack...>>
using TagStack = TypeStack<stack...>;

template<typename type>
static constexpr bool is_tag_stack = is_set_type_stack_v<type> && check_stack_type_constr_v<IsTag, type>;

template<typename type>
concept AnyTagStack = is_tag_stack<type>;

template<typename type>
concept AnyFilledTagStack = AnyTagStack<type> && is_filled_type_stack_v<type>;

template<typename type>
concept EmptyTagStack = AnyTagStack<type> && is_empty_type_stack_v<type>;

template<AnyTagStack stack, AnyTagType tag_type>
static constexpr bool is_typed_tag_stack_v = false;
