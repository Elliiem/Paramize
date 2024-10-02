#pragma once

#include <cstddef>
#include <type_traits>

struct Null;

template<typename... params>
struct NullTempl;

template<typename, typename = void>
constexpr bool is_type_complete_v = false;

template<typename T>
constexpr bool is_type_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;

template<template<typename> typename templ>
concept RawChecker = requires { templ<Null>::_value; } || !is_type_complete_v<templ<Null>>;

template<template<template<typename...> typename> typename templ>
concept RawTemplChecker = requires { templ<NullTempl>::_value; };

template<template<typename> typename top, template<typename> typename... trail>
static constexpr bool is_checker_stack_inner_v = false;

template<template<typename> typename top, template<typename> typename... trail>
    requires(sizeof...(trail) > 0)
static constexpr bool is_checker_stack_inner_v<top, trail...> = RawChecker<top> && is_checker_stack_inner_v<trail...>;

template<template<typename> typename top, template<typename> typename... trail>
    requires(sizeof...(trail) == 0)
static constexpr bool is_checker_stack_inner_v<top, trail...> = RawChecker<top>;

template<template<typename> typename... stack>
static constexpr bool is_raw_checker_stack_v = is_checker_stack_inner_v<stack...>;

template<template<typename> typename... stack>
    requires(sizeof...(stack) > 0)
static constexpr bool is_raw_checker_stack_v<stack...> = is_checker_stack_inner_v<stack...>;

template<template<typename> typename... stack>
    requires(sizeof...(stack) == 0)
static constexpr bool is_raw_checker_stack_v<stack...> = true;

template<template<typename> typename... stack>
concept RawCheckerStack = is_raw_checker_stack_v<stack...>;

template<template<typename> typename a, template<typename> typename b>
static constexpr bool is_same_raw_checker_v = std::is_same_v<a<Null>, b<Null>>;

template<template<typename> typename T, template<typename> typename... stack>
static constexpr bool contains_raw_checker_v = false;

template<template<typename> typename T, template<typename> typename top, template<typename> typename... trail>
static constexpr bool contains_raw_checker_inner_v = false;

template<template<typename> typename T, template<typename> typename... stack>
    requires(sizeof...(stack) > 0)
static constexpr bool contains_raw_checker_v<T, stack...> = contains_raw_checker_inner_v<T, stack...>;

template<template<typename> typename T, template<typename> typename... stack>
    requires(sizeof...(stack) == 0)
static constexpr bool contains_raw_checker_v<T, stack...> = false;

template<template<typename> typename T, template<typename> typename top, template<typename> typename... trail>
    requires RawChecker<top> && (sizeof...(trail) > 0)
static constexpr bool contains_raw_checker_inner_v<T, top, trail...> = is_same_raw_checker_v<T, top> || contains_raw_checker_inner_v<T, trail...>;

template<template<typename> typename T, template<typename> typename top, template<typename> typename... trail>
    requires RawChecker<top> && (sizeof...(trail) == 0)
static constexpr bool contains_raw_checker_inner_v<T, top, trail...> = is_same_raw_checker_v<T, top>;

template<typename T, template<typename> typename top, template<typename> typename... trail>
    requires RawChecker<top> && RawCheckerStack<trail...>
static constexpr bool t_constr_inner_v = false;

template<typename T, template<typename> typename top, template<typename> typename... trail>
    requires RawChecker<top> && RawCheckerStack<trail...> && (sizeof...(trail) > 0)
static constexpr bool t_constr_inner_v<T, top, trail...> = top<T>::_value || t_constr_inner_v<T, trail...>;

template<typename T, template<typename> typename top, template<typename> typename... trail>
    requires RawChecker<top> && RawCheckerStack<trail...> && (sizeof...(trail) == 0)
static constexpr bool t_constr_inner_v<T, top, trail...> = top<T>::_value;

template<typename T, template<typename> typename... options>
    requires RawCheckerStack<options...>
static constexpr bool t_constr_v = false;

template<typename T, template<typename> typename... options>
    requires RawCheckerStack<options...> && (sizeof...(options) > 0)
static constexpr bool t_constr_v<T, options...> = t_constr_inner_v<T, options...>;

template<typename T, template<typename> typename... options>
    requires RawCheckerStack<options...> && (sizeof...(options) == 0)
static constexpr bool t_constr_v<T, options...> = false;

template<template<typename> typename... opt>
struct Checker {
    template<typename T>
        requires RawCheckerStack<opt...>
    static constexpr bool _value = t_constr_v<T, opt...>;
};

template<typename T>
static constexpr bool is_checker_v = false;

template<template<typename> typename... opt>
static constexpr bool is_checker_v<Checker<opt...>> = true;

template<typename T>
concept AnyChecker = is_checker_v<T>;

template<typename T, typename checker>
concept TConstr = AnyChecker<checker> && checker::template _value<T>;

template<AnyChecker a, AnyChecker b>
struct MergeCheckersHelper;

template<AnyChecker a, template<typename> typename... b_exp>
struct MergeCheckersHelper<a, Checker<b_exp...>> {
    template<AnyChecker a_inner>
    struct MergeCheckersHelperInner;

    template<template<typename> typename... a_exp>
    struct MergeCheckersHelperInner<Checker<a_exp...>> {
        using _value = Checker<a_exp..., b_exp...>;
    };

    using _value = MergeCheckersHelperInner<a>::_value;
};

template<AnyChecker checker>
struct DedupCheckerHelper;

template<size_t to_eval, template<typename> typename top, template<typename> typename... trail>
struct DedupCheckerHelperInner;

template<template<typename> typename... stack>
struct DedupCheckerHelper<Checker<stack...>> {
    using _value = DedupCheckerHelperInner<sizeof...(stack), stack...>::_value;
};

template<size_t to_eval, template<typename> typename top, template<typename> typename... trail>
    requires(to_eval > 0) && (sizeof...(trail) > 0)
struct DedupCheckerHelperInner<to_eval, top, trail...> {
    static constexpr bool _is_duplicate = contains_raw_checker_v<top, trail...>;

    using _next =
        std::conditional_t<_is_duplicate, DedupCheckerHelperInner<to_eval - 1, trail...>, DedupCheckerHelperInner<to_eval - 1, trail..., top>>;

    using _value = _next::_value;
};

template<size_t to_eval, template<typename> typename top, template<typename> typename... trail>
    requires(to_eval > 0) && (sizeof...(trail) == 0)
struct DedupCheckerHelperInner<to_eval, top, trail...> {
    using _value = Checker<top>;
};

template<size_t to_eval, template<typename> typename top, template<typename> typename... trail>
    requires(to_eval == 0)
struct DedupCheckerHelperInner<to_eval, top, trail...> {
    using _value = Checker<top, trail...>;
};

template<AnyChecker checker>
using dedupChecker = DedupCheckerHelper<checker>::_value;

template<AnyChecker a, AnyChecker b>
using mergeCheckers = dedupChecker<typename MergeCheckersHelper<a, b>::_value>;

template<AnyChecker a, AnyChecker b>
struct AreSimilarCheckersHelper;

template<AnyChecker a, template<typename> typename... b_exp>
struct AreSimilarCheckersHelper<a, Checker<b_exp...>> {
    template<AnyChecker>
    struct AreSimilarCheckersHelperInner;

    template<template<typename> typename a_top, template<typename> typename... a_trail>
        requires(sizeof...(a_trail) > 0)
    struct AreSimilarCheckersHelperInner<Checker<a_top, a_trail...>> {
        static constexpr bool _value = contains_raw_checker_v<a_top, b_exp...> || AreSimilarCheckersHelperInner<Checker<a_trail...>>::_value;
    };

    template<template<typename> typename a_top, template<typename> typename... a_trail>
        requires(sizeof...(a_trail) == 0)
    struct AreSimilarCheckersHelperInner<Checker<a_top, a_trail...>> {
        static constexpr bool _value = contains_raw_checker_v<a_top, b_exp...>;
    };

    static constexpr bool _value = AreSimilarCheckersHelperInner<a>::_value;
};

template<AnyChecker a, AnyChecker b>
static constexpr bool are_similar_checkers_v = AreSimilarCheckersHelper<a, b>::_value;

template<AnyChecker a, AnyChecker b>
static constexpr bool are_equivalent_checkers_v = are_similar_checkers_v<a, b> && are_similar_checkers_v<b, a>;

template<AnyChecker is, AnyChecker comparable>
struct Comparator {
    using _base = Comparator<is, comparable>;

    using _strong_is = is;
    using _weak_is = mergeCheckers<is, comparable>;
};

template<typename T>
concept AnyComparator = requires {
    typename T::_base;
    std::is_base_of_v<typename T::_base, T>;
};

template<typename T, typename cmp>
concept CmpWeakTConstr = AnyComparator<cmp> && cmp::_weak_is::template _value<T>;

template<typename T, typename cmp>
concept CmpStrongTConstr = AnyComparator<cmp> && cmp::_strong_is::template _value<T>;

template<typename T, typename cmp, template<typename> typename spec>
concept CmpWeakSpecTConstr = AnyComparator<cmp> && RawChecker<spec> && are_similar_checkers_v<Checker<spec>, typename cmp::_weak_is> &&
                             CmpWeakTConstr<T, cmp> && spec<T>::_value;

template<typename T, typename is, typename comparable>
concept ConstrCmp = AnyChecker<is> && AnyChecker<comparable> && std::is_base_of_v<Comparator<is, comparable>, T>;

template<AnyChecker is, AnyChecker comparable, ConstrCmp<is, comparable> cmp>
struct TypeView;

template<typename T>
static constexpr bool is_type_view = false;

template<AnyChecker is, AnyChecker comparable, ConstrCmp<is, comparable> cmp>
static constexpr bool is_type_view<TypeView<is, comparable, cmp>> = true;

template<AnyChecker is, AnyChecker comparable, ConstrCmp<is, comparable> cmp>
struct Type {
    using _type = TypeView<is, comparable, cmp>;
};

template<AnyChecker is, AnyChecker comparable, ConstrCmp<is, comparable> cmp>
struct TypeView {
    using _is = is;
    using _cmp = cmp;

    using _type = Type<is, comparable, cmp>;
};

template<typename T>
concept AnyTypeInstance = requires {
    typename T::_type;
    std::is_base_of_v<typename T::_type, T>;
};

template<typename T>
static constexpr bool is_type_v = false;

template<AnyChecker is, AnyChecker comparable, ConstrCmp<is, comparable> cmp>
static constexpr bool is_type_v<Type<is, comparable, cmp>> = true;

template<AnyTypeInstance a, TConstr<typename a::_type::_cmp::_weak_is> b>
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
concept ParamTag = TConstr<type, Checker<IsParameter, IsTag>> && IsTag<type>::_value;

struct ParameterCmp : Comparator<Checker<IsParameter>, Checker<IsTag>> {
    template<CmpStrongTConstr<_base> a, CmpWeakTConstr<_base> b>
    static constexpr bool _value = false;

    template<CmpStrongTConstr<_base> a, CmpWeakSpecTConstr<_base, IsTag> b>
    static constexpr bool _value<a, b> = std::is_same_v<tagof<a>, b>;

    template<CmpStrongTConstr<_base> a, CmpWeakSpecTConstr<_base, IsParameter> b>
    static constexpr bool _value<a, b> = std::is_same_v<a, b>;
};

template<typename tag>
struct Parameter : Type<Checker<IsParameter>, Checker<IsTag>, ParameterCmp> {
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

static_assert(cmp<A, ATag>);
