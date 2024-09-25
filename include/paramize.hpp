#pragma once

#include "parse_parameters.hpp"

#include <cmath>
#include <cstdint>
#include <type_traits>

namespace ColorCompTag {
    struct R : ParameterTag {};
    struct G : ParameterTag {};
    struct B : ParameterTag {};

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
    struct A : ParameterTag {};
    struct B : ParameterTag {};
    struct C : ParameterTag {};

    using All = TagStack<A, B, C>;
} // namespace CharCompTag

namespace CharComp {
    struct A : Parameter<CharCompTag::A> {};
    struct B : Parameter<CharCompTag::B> {};
    struct C : Parameter<CharCompTag::C> {};
} // namespace CharComp

namespace Regions {
    struct Char : RegionTag {};
    struct Color : RegionTag {};
}; // namespace Regions

using char_region = SetParamRegion<Regions::Char, CharCompTag::All>;
using color_region = SetParamRegion<Regions::Color, ColorCompTag::All, ParamStack<ColorComp::R<0>, ColorComp::G<0>, ColorComp::B<0>>>;

using regions = ParamRegionStack<color_region, char_region>;

static_assert(std::is_same_v<parseParameters<regions, ParamStack<ColorComp::G<255>, CharComp::A, CharComp::B, CharComp::C>>,void>);

template<typename... params>
    requires ValidParameters<regions, params...>
struct Foo {};

static constexpr Foo<ColorComp::G<255>, CharComp::A, CharComp::B, CharComp::C> foo;
