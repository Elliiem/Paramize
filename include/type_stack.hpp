#pragma once

#include "tag.hpp"
#include <cstddef>
#include <type_traits>

struct Null;

template<typename type>
static constexpr bool is_null_v = false;

template<>
static constexpr bool is_null_v<Null> = true;

template<typename type>
concept Opt = true;

template<typename type>
concept Nopt = !is_null_v<type>;

template<typename type>
static constexpr bool is_value_type_v = requires {type::_value;};

template<typename type>
concept AnyValueType = is_value_type_v<type>;

template<Nopt... stack>
struct TypeStack;

template<typename type, typename top>
concept StackTrailItem = !is_null_v<top>;

template<Opt top = Null, StackTrailItem<top>... trail>
struct Split {
    using _top = top;
    using _trail = TypeStack<trail...>;
};

template<typename type>
static constexpr bool is_type_stack_v = false;

template<Nopt... stack>
static constexpr bool is_type_stack_v<TypeStack<stack...>> = true;

template<typename type>
concept AnyTypeStack = is_type_stack_v<type>;

template<typename>
static constexpr bool is_empty_type_stack_v = false;

template<>
static constexpr bool is_empty_type_stack_v<TypeStack<>> = true;

template<typename type>
static constexpr bool is_filled_type_stack_v = is_type_stack_v<type> && !is_empty_type_stack_v<type>;

template<typename type>
concept EmptyTypeStack = AnyTypeStack<type> && is_empty_type_stack_v<type>;

template<typename type>
concept AnyFilledTypeStack = AnyTypeStack<type> && is_filled_type_stack_v<type>;

template<Nopt... items>
struct TypeStack {
    using _this = TypeStack<items...>;

    using _top = Split<items...>::_top; 
    using _trail = Split<items...>::_trail;

    static constexpr size_t _lenght = sizeof...(items);

    template<size_t n, AnyTypeStack stack>
    struct PopHelper;

    template<size_t n, AnyFilledTypeStack stack>
    struct PopHelper<n, stack> {
        using _value = PopHelper<n - 1, typename stack::_trail>::_value;
    };

    template<size_t n, EmptyTypeStack stack>
    struct PopHelper<n, stack> {
        using _value = stack; 
    };

    template<AnyTypeStack stack>
    struct PopHelper<0, stack> {
        using _value = stack; 
    };

    template<size_t n>
    using pop = PopHelper<n, _this>::_value;

    template<Nopt... stack>
    using push = TypeStack<stack..., items...>;

    template<AnyTypeStack cur, size_t i>
    struct AtHelper {
        using _next = AtHelper<typename cur::template pop<1>, i - 1>;
    };

    template<AnyTypeStack stack>
    struct AtHelper<stack, 0> {
        using _value = stack::_top;
    };

    template<EmptyTypeStack stack, size_t i>
    struct AtHelper<stack, i> {
        using _value = Null;
    };

    template<size_t i>
    using at = AtHelper<_this, i>;

    //  FIXME: Constrain this mf
    template<Nopt item, AnyTypeStack stack, template<Opt, Opt> typename cmp>
    struct ContainsHelper {
        using _next = ContainsHelper<item, typename stack::template pop<1>, cmp>;

        static constexpr bool _value = cmp<typename stack::_top, item>::_value || _next::_value;
    };

    template<Nopt item, EmptyTypeStack stack, template<Opt, Opt> typename cmp>
    struct ContainsHelper<item, stack, cmp> {
        static constexpr bool _value = false;
    };

    template<typename a, typename b>
    struct SameCmp {
        static constexpr bool _value = std::is_same_v<a, b>;
    };

    template<Nopt item, template<Opt, Opt> typename eval = SameCmp>
    static constexpr bool contains = ContainsHelper<item, _this, eval>::_value;
};

template<template<typename> typename constr, AnyTypeStack cur>
struct CheckStackTypeConstrHelper;

template<template<typename> typename constr, AnyFilledTypeStack cur>
struct CheckStackTypeConstrHelper<constr, cur> {
    using _next = CheckStackTypeConstrHelper<constr, typename cur::template pop<1>>;

    static constexpr bool _value = constr<typename cur::_top>::_value && _next::_value;
};

template<template<typename> typename constr, EmptyTypeStack cur>
struct CheckStackTypeConstrHelper<constr, cur> {
    static constexpr bool _value = true;
};

template<template<typename> typename constr, AnyTypeStack stack>
static constexpr bool check_stack_type_constr_v = CheckStackTypeConstrHelper<constr, stack>::_value;


template<AnyTypeStack stack, AnyTypeStack found>
struct StackItemsUniqueHelper;

template<AnyFilledTypeStack stack, AnyTypeStack found>
struct StackItemsUniqueHelper<stack, found> {
    using _next = StackItemsUniqueHelper<typename stack::template pop<1>, typename found::template push<typename stack::_top>>;

    static constexpr bool _value = !found::template contains<typename stack::_top> && _next::_value;
};

template<EmptyTypeStack stack, AnyTypeStack found>
struct StackItemsUniqueHelper<stack, found> {
    static constexpr bool _value = true;
};

template<typename type>
static constexpr bool is_set_type_stack_v = is_type_stack_v<type> && StackItemsUniqueHelper<type, TypeStack<>>::_value;

template<typename type>
concept AnySetTypeStack = is_set_type_stack_v<type>;



