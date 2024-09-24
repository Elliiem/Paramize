#pragma once

#include "parse.hpp"

#include <cmath>
#include <cstdint>

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

using regions = ParamRegionStack<color_region, char_region>;

template<typename... params>
    requires ValidParameters<regions, params...>
struct Foo {

};

static constexpr Foo<ColorComp::G<255>, CharComp::A, CharComp::B, CharComp::C> foo;
