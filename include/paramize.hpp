#pragma once

#include "parse.hpp"
#include <cstdint>

// ########################

namespace ColorCompTag {
struct R;
struct G;
struct B;

using All = TagStack<R, G, B>;
} // namespace ColorCompTag

namespace ColorComp {
template<uint8_t value = 0>
struct R : Parameter<ColorCompTag::R> {
    static constexpr uint8_t _value = value;
};

template<uint8_t value = 0>
struct G : Parameter<ColorCompTag::G> {
    static constexpr uint8_t _value = value;
};

template<uint8_t value = 0>
struct B : Parameter<ColorCompTag::B> {
    static constexpr uint8_t _value = value;
};

} // namespace ColorComp

namespace CharCompTag {
struct A;
struct B;
struct C;

using All = TagStack<A, B, C>;
} // namespace CharCompTag

namespace CharComp {
struct A : Parameter<CharCompTag::A> {};
struct B : Parameter<CharCompTag::B> {};
struct C : Parameter<CharCompTag::C> {};
} // namespace CharComp

using char_region = SetParamRegion<CharCompTag::All>;
using color_region = SetParamRegion<ColorCompTag::All, ParamStack<ColorComp::R<0>, ColorComp::G<0>, ColorComp::B<0>>>;
using params = ParamStack<ColorComp::G<255>>;

using res = parseRegion<color_region, params>;

static_assert(std::is_same_v<res, void>);

// A single constraint of parameters
// template<AnyTagSet constr, size_t max_param_c>
// struct SetParameterRegion {};
//
// -- . - . --
// struct ListParameterRegion {};
//
// template<...>
// concept AnyParameterRegion
//
// template<AnyParameterRegion... regions>
// using ParameterRegionStack = TypeStack<regions...>;
//
// template<AnyFilledTypeStack stack>
// struct IsParameterRegionStackHelper {
//     using _next = IsParameterStackHelper<typeanme stack::pop>
//
//     using _value = AnyParameterRegion<stack::_top> && _next::_value;
// };
//
// template<AnyEmptyTypeStack stack>
// struct IsParameterRegionStackHelper<stack> {
//    using _value = AnyParameterRegion<stack::_top>;
// }
//
// template<typename type>
// constexpr bool is_parameter_region_stack = IsParameterRegionStackHelper<...>::_value;
//
// teplate<typename type>
// concept AnyParameterRegionStack =
//
// template<AnyParameterRegionStack regions>
// concept ConstrainedParams = ...
//
// template<AnyTagSet constr, ConstrainedParmeterValue<constr>... stack>
// struct ParameterStack {
//     using _top = stack_top_t<stack>
//
//
//     ...
// }

// template<typename... params>
//     requires TaggedParamSet<color_tag_set, params...>
// struct Color{
//
// };
