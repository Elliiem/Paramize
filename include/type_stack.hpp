#pragma once

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

template<typename type, typename top>
concept StackTrailItem = !is_null_v<top>;

template<Opt top = Null, StackTrailItem<top>... trail>
struct GetStackTopHelper {
    using _value = top;
};

template<Nopt... stack>
using getStackTop = GetStackTopHelper<stack...>::_value;

// ########################

template<Nopt... stack>
struct TypeStack;

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
concept AnyEmptyTypeStack = AnyTypeStack<type> && is_empty_type_stack_v<type>;

template<typename type>
concept AnyFilledTypeStack = AnyTypeStack<type> && is_filled_type_stack_v<type>;

template<Nopt... stack>
struct TypeStack {
    using _this = TypeStack<stack...>;
    using _top = getStackTop<stack...>;

    static constexpr size_t _lenght = sizeof...(stack);

    template<Opt top = Null, StackTrailItem<top>... trail>
    struct PopHelper {
        using _value = TypeStack<trail...>;
    };

    using pop = PopHelper<stack...>::_value;

    template<size_t n, AnyTypeStack cur>
    struct PopNHelper;

    template<size_t n, AnyFilledTypeStack cur>
    struct PopNHelper<n, cur> {
        using _value = PopNHelper<n -1, typename cur::pop>;
    };

    template<size_t n, AnyEmptyTypeStack cur>
    struct PopNHelper<n, cur> {
        using _value = cur; 
    };

    template<AnyTypeStack cur>
    struct PopNHelper<0, cur> {
        using _value = cur; 
    };

    template<size_t n>
    using popN = PopNHelper<n, _this>;

    template<Nopt... items>
    using append = TypeStack<stack..., items...>;

    template<Nopt type, AnyTypeStack cur>
    struct ContainsHelper;

    template<Nopt type, AnyFilledTypeStack cur>
    struct ContainsHelper<type, cur> {
        using _next = ContainsHelper<type, typename cur::pop>;

        static constexpr bool _value = std::is_same_v<type, typename cur::_top> || _next::_value;
    };

    template<Nopt type, AnyEmptyTypeStack cur>
    struct ContainsHelper<type, cur> {
        static constexpr bool _value = false;
    };

    template<Nopt type>
    static constexpr bool contains_v = ContainsHelper<type, _this>::_value;
};

template<template<typename> typename constr, AnyTypeStack cur>
struct CheckStackTypeConstrHelper;

template<template<typename> typename constr, AnyFilledTypeStack cur>
struct CheckStackTypeConstrHelper<constr, cur> {
    using _next = CheckStackTypeConstrHelper<constr, typename cur::pop>;

    static constexpr bool _value = constr<typename cur::_top>::_value && _next::_value;
};

template<template<typename> typename constr, AnyEmptyTypeStack cur>
struct CheckStackTypeConstrHelper<constr, cur> {
    static constexpr bool _value = true;
};

template<template<typename> typename constr, AnyTypeStack stack>
static constexpr bool check_stack_type_constr_v = CheckStackTypeConstrHelper<constr, stack>::_value;


template<AnyTypeStack stack, AnyTypeStack found>
struct StackItemsUniqueHelper;

template<AnyFilledTypeStack stack, AnyTypeStack found>
struct StackItemsUniqueHelper<stack, found> {
    using _next = StackItemsUniqueHelper<typename stack::pop, typename found::template append<typename stack::_top>>;

    static constexpr bool _value = !found::template contains_v<typename stack::_top> && _next::_value;
};

template<AnyEmptyTypeStack stack, AnyTypeStack found>
struct StackItemsUniqueHelper<stack, found> {
    static constexpr bool _value = true;
};

template<typename type>
static constexpr bool is_set_type_stack_v = is_type_stack_v<type> && StackItemsUniqueHelper<type, TypeStack<>>::_value;

template<typename type>
concept AnySetTypeStack = is_set_type_stack_v<type>;

