#pragma once

#include <type_traits>
#include <vector>

struct Null;

template<typename... params>
struct NullTempl;

template<typename params>
struct NullCheckerTempl {};

template<template<typename> typename templ>
static constexpr bool is_null_checker_templ_v = false;

template<>
static constexpr bool is_null_checker_templ_v<NullCheckerTempl> = true;

template<template<typename...> typename templ>
static constexpr bool is_null_t_templ_v = false;

template<>
static constexpr bool is_null_t_templ_v<NullCheckerTempl> = true;

template<template<typename...> typename templ>
struct IsNullTTempl {
    static constexpr bool _value = is_null_t_templ_v<templ>;
};

template<template<typename> typename templ>
concept Checker = requires { templ<Null>::_value; };

template<template<template<typename...> typename> typename templ>
concept TemplChecker = requires { templ<NullTempl>::_value; };

/*
template<template<template<typename...> typename> typename checker, template<typename... > typename top, template<typename... > typename... trail>
    requires TemplChecker<checker>
static constexpr bool contains_templ_inner_v = false;

template<template<template<typename...> typename> typename checker, template<typename... > typename top, template<typename... > typename... trail>
    requires TemplChecker<checker> && (sizeof...(trail) > 0)
static constexpr bool contains_templ_inner_v<checker, top, trail...> = checker<top>::_value || contains_templ_inner_v<checker, trail...>;

template<template<template<typename...> typename> typename checker, template<typename... > typename top, template<typename... > typename... trail>
    requires TemplChecker<checker> && (sizeof...(trail) == 0)
static constexpr bool contains_templ_inner_v<checker, top, trail...> = checker<top>::_value;

template<template<template<typename...> typename> typename checker, template<typename... > typename... stack>
    requires TemplChecker<checker>
static constexpr bool contains_templ_v = false;

template<template<template<typename...> typename> typename checker, template<typename... > typename... stack>
    requires TemplChecker<checker> && (sizeof...(stack) > 0)
static constexpr bool contains_templ_v<checker, stack...> = contains_templ_inner_v<checker, stack...>;

template<template<template<typename...> typename> typename checker, template<typename... > typename... stack>
    requires TemplChecker<checker> && (sizeof...(stack) == 0)
static constexpr bool contains_templ_v<checker, stack...> = false;
*/

template<template<typename> typename top, template<typename> typename... trail>
static constexpr bool is_checker_stack_inner_v = false;

template<template<typename> typename top, template<typename> typename... trail>
    requires(sizeof...(trail) > 0)
static constexpr bool is_checker_stack_inner_v<top, trail...> = Checker<top> && is_checker_stack_inner_v<trail...>;

template<template<typename> typename top, template<typename> typename... trail>
    requires(sizeof...(trail) == 0)
static constexpr bool is_checker_stack_inner_v<top, trail...> = Checker<top>;

template<template<typename> typename... stack>
static constexpr bool is_checker_stack_v = is_checker_stack_inner_v<stack...>;

template<template<typename> typename... stack>
    requires(sizeof...(stack) > 0)
static constexpr bool is_checker_stack_v<stack...> = is_checker_stack_inner_v<stack...>;

template<template<typename> typename... stack>
    requires(sizeof...(stack) == 0)
static constexpr bool is_checker_stack_v<stack...> = true;

template<template<typename> typename... stack>
concept CheckerStack = is_checker_stack_v<stack...>;

template<typename T, template<typename> typename top, template<typename> typename... trail>
    requires Checker<top> && CheckerStack<trail...>
static constexpr bool t_constr_inner_v = false;

template<typename T, template<typename> typename top, template<typename> typename... trail>
    requires Checker<top> && CheckerStack<trail...> && (sizeof...(trail) > 0)
static constexpr bool t_constr_inner_v<T, top, trail...> = top<T>::_value || t_constr_inner_v<T, trail...>;

template<typename T, template<typename> typename top, template<typename> typename... trail>
    requires Checker<top> && CheckerStack<trail...> && (sizeof...(trail) == 0)
static constexpr bool t_constr_inner_v<T, top, trail...> = top<T>::_value;

template<typename T, template<typename> typename... options>
    requires CheckerStack<options...>
static constexpr bool t_constr_v = false;

template<typename T, template<typename> typename... options>
    requires CheckerStack<options...> && (sizeof...(options) > 0)
static constexpr bool t_constr_v<T, options...> = t_constr_inner_v<T, options...>;

template<typename T, template<typename> typename... options>
    requires CheckerStack<options...> && (sizeof...(options) == 0)
static constexpr bool t_constr_v<T, options...> = false;

template<typename T, template<typename> typename... options>
concept TConstr = t_constr_v<T, options...>;

template<template<typename> typename... stack>
struct SplitCheckerStack;

template<template<typename> typename... opt>
struct CompChecker {
    // NOTE: We have to defer requirements to the actual use of the template otherwise you cant
    // declare types very well. The `is` checker most of the time requires the type its
    // corresponding to, so you need to forward declare the checker to define your type, and then
    // you can define the checker. As the checker is forward declared it does not yet have the
    // _value field and wont pass Checker<>, So the ConvChecker "requirement" in Type wont be happy
    // if ConvChecker itself requires a CheckerStack, as at the time of creating this type the `is`
    // checker is not yet a checker

    template<typename T>
        requires CheckerStack<opt...>
    struct Checker {
        // TODO: Can we get parent just from the type?
        using _parent = CompChecker<opt...>;

        static constexpr bool _value = TConstr<T, opt...>;
    };
};

template<typename T>
static constexpr bool is_comp_checker_v = false;

template<template<typename> typename... opt>
static constexpr bool is_comp_checker_v<CompChecker<opt...>> = true;

template<typename T>
concept AnyCompChecker = is_comp_checker_v<T>;

template<template<typename> typename T>
concept CompCheckerChild = requires { typename T<Null>::_parent; };

template<template<typename> typename a, template<typename> typename b>
static constexpr bool is_same_checker_v = std::is_same_v<a<Null>, b<Null>>;

template<template<typename> typename T, template<typename> typename... stack>
static constexpr bool contains_checker_v = false;

template<template<typename> typename T, template<typename> typename top, template<typename> typename... trail>
static constexpr bool contains_checker_inner_v = false;

template<template<typename> typename T, AnyCompChecker parent>
static constexpr bool contains_checker_inner_expand_v = false;

template<template<typename> typename T, template<typename> typename... stack>
    requires(sizeof...(stack) > 0)
static constexpr bool contains_checker_v<T, stack...> = contains_checker_inner_v<T, stack...>;

template<template<typename> typename T, template<typename> typename... stack>
    requires(sizeof...(stack) == 0)
static constexpr bool contains_checker_v<T, stack...> = false;

template<template<typename> typename T, template<typename> typename top, template<typename> typename... trail>
    requires Checker<top> && (!CompCheckerChild<top>) && (sizeof...(trail) > 0)
static constexpr bool contains_checker_inner_v<T, top, trail...> = is_same_checker_v<T, top> || contains_checker_inner_v<T, trail...>;

template<template<typename> typename T, template<typename> typename top, template<typename> typename... trail>
    requires Checker<top> && (!CompCheckerChild<top>) && (sizeof...(trail) == 0)
static constexpr bool contains_checker_inner_v<T, top, trail...> = is_same_checker_v<T, top>;

template<template<typename> typename T, template<typename> typename top, template<typename> typename... trail>
    requires Checker<top> && CompCheckerChild<top> && (sizeof...(trail) > 0)
static constexpr bool contains_checker_inner_v<T, top, trail...> =
    contains_checker_inner_expand_v<T, typename top<Null>::_parent> || contains_checker_inner_v<T, trail...>;

template<template<typename> typename T, template<typename> typename top, template<typename> typename... trail>
    requires Checker<top> && CompCheckerChild<top> && (sizeof...(trail) == 0)
static constexpr bool contains_checker_inner_v<T, top, trail...> = contains_checker_inner_expand_v<T, typename top<Null>::_parent>;

template<template<typename> typename T, template<typename> typename... stack>
static constexpr bool contains_checker_inner_expand_v<T, CompChecker<stack...>> = contains_checker_v<T, stack...>;

template<AnyCompChecker a, AnyCompChecker b>
static constexpr bool are_similar_checkers_v = false;

template<AnyCompChecker b, template<typename> typename top, template<typename> typename... trail>
static constexpr bool are_similar_checkers_inner_v = false;

template<AnyCompChecker b, template<typename> typename... stack>
    requires(sizeof...(stack) > 0)
static constexpr bool are_similar_checkers_v<CompChecker<stack...>, b> = are_similar_checkers_inner_v<b, stack...>;

template<AnyCompChecker b, template<typename> typename... stack>
    requires(sizeof...(stack) == 0)
static constexpr bool are_similar_checkers_v<CompChecker<stack...>, b> = true;

template<AnyCompChecker b, template<typename> typename top, template<typename> typename... trail>
    requires(sizeof...(trail) > 0) && (!CompCheckerChild<top>)
static constexpr bool are_similar_checkers_inner_v<b, top, trail...> =
    contains_checker_v<top, b::template Checker> && are_similar_checkers_inner_v<b, trail...>;

template<AnyCompChecker b, template<typename> typename top, template<typename> typename... trail>
    requires(sizeof...(trail) == 0) && (!CompCheckerChild<top>)
static constexpr bool are_similar_checkers_inner_v<b, top, trail...> = contains_checker_v<top, b::template Checker>;

template<AnyCompChecker b, template<typename> typename top, template<typename> typename... trail>
    requires(sizeof...(trail) > 0) && CompCheckerChild<top>
static constexpr bool are_similar_checkers_inner_v<b, top, trail...> =
    are_similar_checkers_v<typename top<Null>::_parent, b> && are_similar_checkers_inner_v<b, trail...>;

template<AnyCompChecker b, template<typename> typename top, template<typename> typename... trail>
    requires(sizeof...(trail) == 0) && CompCheckerChild<top>
static constexpr bool are_similar_checkers_inner_v<b, top, trail...> = are_similar_checkers_v<typename top<Null>::_parent, b>;

template<AnyCompChecker a, AnyCompChecker b>
static constexpr bool are_equivalent_checkers_v = are_similar_checkers_v<a, b> && are_similar_checkers_v<b, a>;

template<AnyCompChecker is, AnyCompChecker comparable>
struct Comparator {
    using _base = Comparator<is, comparable>;

    using _strong_is = is;
    using _weak_is = CompChecker<is::template Checker, comparable::template Checker>;
};

template<typename T>
concept AnyComparator = requires {
    typename T::_base;
    std::is_base_of_v<typename T::_base, T>;
};

template<typename T, typename cmp>
concept CmpWeakTConstr = AnyComparator<cmp> && cmp::_weak_is::template Checker<T>::_value;

template<typename T, typename cmp>
concept CmpStrongTConstr = AnyComparator<cmp> && cmp::_strong_is::template Checker<T>::_value;

template<typename T, typename cmp, template<typename> typename spec>
concept CmpWeakSpecTConstr = AnyComparator<cmp> && Checker<spec> && are_similar_checkers_v<CompChecker<spec>, typename cmp::_weak_is> &&
                         CmpWeakTConstr<T, cmp> && spec<T>::_value;

template<typename T, typename is, typename comparable>
concept ConstrCmp = AnyCompChecker<is> && AnyCompChecker<comparable> && std::is_base_of_v<Comparator<is, comparable>, T>;

template<AnyCompChecker is, AnyCompChecker comparable, ConstrCmp<is, comparable> cmp>
struct TypeView;

template<typename T>
static constexpr bool is_type_view = false;

template<AnyCompChecker is, AnyCompChecker comparable, ConstrCmp<is, comparable> cmp>
static constexpr bool is_type_view<TypeView<is, comparable, cmp>> = true;

template<AnyCompChecker is, AnyCompChecker comparable, ConstrCmp<is, comparable> cmp>
struct Type {
    using _type = TypeView<is, comparable, cmp>;
};

template<AnyCompChecker is, AnyCompChecker comparable, ConstrCmp<is, comparable> cmp>
struct TypeView {
    using _is = is;
    using _cmp = cmp;

    using _type = Type<is, comparable, cmp>;
};


template<typename T>
concept Typey = requires {
    typename T::_type;
    is_type_view<typename T::_type>;
};

template<typename T>
static constexpr bool is_type_v = false;

template<AnyCompChecker is, AnyCompChecker comparable, ConstrCmp<is, comparable> cmp>
static constexpr bool is_type_v<Type<is, comparable, cmp>> = true;

template<Typey a, TConstr<a::_type::_cmp::_weak_is::template Checker> b>
static constexpr bool cmp = a::_type::_cmp::template _value<a, b>;

struct Tag {};

template<typename T>
struct IsTag {
    static constexpr bool _value = std::is_base_of_v<Tag, T>;
};

template<typename type>
concept Tagged = requires { typename type::_tag; };

template<Tagged T>
using tagof = T::_tag;

template<typename type>
struct IsParameter;

template<typename type>
concept ParamTag = TConstr<type, IsParameter, IsTag> && IsTag<type>::_value;

struct ParameterCmp : Comparator<CompChecker<IsParameter>, CompChecker<IsTag>> {
    template<CmpStrongTConstr<_base> a, CmpWeakTConstr<_base> b>
    static constexpr bool _value = false;

    template<CmpStrongTConstr<_base> a, CmpWeakSpecTConstr<_base, IsTag> b>
    static constexpr bool _value<a, b> = std::is_same_v<tagof<a>, b>;

    template<CmpStrongTConstr<_base> a, CmpWeakSpecTConstr<_base, IsParameter> b>
    static constexpr bool _value<a, b> = std::is_same_v<a, b>;
};

template<typename tag>
struct Parameter : Type<CompChecker<IsParameter>, CompChecker<IsTag>, ParameterCmp> {
    using _tag = tag;
};

template<typename type>
struct IsParameter {
    static constexpr bool _value = false;
};

template<typename type>
    requires Tagged<type>
struct IsParameter<type> {
    static constexpr bool _value = std::is_base_of_v<Parameter<typename type::_tag>, type>;
};

struct ATag : Tag {};
struct BTag : Tag {};

struct A : Parameter<ATag> {};
struct B : Parameter<BTag> {};

template<TConstr<IsParameter>>
struct Foo {};

static_assert(cmp<A, A>);
