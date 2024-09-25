#pragma once

#include "type_stack.hpp"

struct Ok;

template<typename type>
static constexpr bool is_okay_v = false;

template<>
static constexpr bool is_okay_v<Ok> = true;

template<typename type>
concept Okay = is_okay_v<type>;

template<AnyTypeStack results>
struct MergeResultsHelper {
    using _cur = results::_top;

    using _next = MergeResultsHelper<typename results::template pop<1>>;

    using _value = std::conditional_t<is_okay_v<_cur>, typename _next::_value, _cur>;
};

template<EmptyTypeStack results>
struct MergeResultsHelper<results> {
    using _value = Ok;
};

template<typename... results>
using mergeResults = MergeResultsHelper<TypeStack<results...>>::_value;
