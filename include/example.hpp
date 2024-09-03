#pragma once

#include "paramize.hpp"

#include <cstdint>

namespace ColorTags {
struct R;
struct G;
struct B;
struct A;
} // namespace ColorTags

namespace ColorComp {
template<uint8_t value>
struct R : Paramize::ParamValue<ColorTags::R> {
    static constexpr uint8_t _value = value;
};
template<uint8_t value>
struct G : Paramize::ParamValue<ColorTags::G> {
    static constexpr uint8_t _value = value;
};
template<uint8_t value>
struct B : Paramize::ParamValue<ColorTags::B> {
    static constexpr uint8_t _value = value;
};
template<uint8_t value>
struct A : Paramize::ParamValue<ColorTags::A> {
    static constexpr uint8_t _value = value;
};
} // namespace ColorComp

using ColorTagSet = Paramize::TagSet<ColorTags::R, ColorTags::G, ColorTags::B, ColorTags::A>;

template<Paramize::OptUniqueParam<ColorTagSet> C1 = void, Paramize::OptUniqueParam<ColorTagSet, C1> C2 = void,
    Paramize::OptUniqueParam<ColorTagSet, C1, C2> C3 = void, Paramize::OptUniqueParam<ColorTagSet, C1, C2, C3> C4 = void>
struct Color {
    static constexpr uint8_t _r = Paramize::tag_value<ColorTags::R, ColorComp::R<0>, C1, C2, C3, C4>::_value;
    static constexpr uint8_t _g = Paramize::tag_value<ColorTags::G, ColorComp::G<0>, C1, C2, C3, C4>::_value;
    static constexpr uint8_t _b = Paramize::tag_value<ColorTags::B, ColorComp::B<0>, C1, C2, C3, C4>::_value;
    static constexpr uint8_t _a = Paramize::tag_value<ColorTags::A, ColorComp::A<0>, C1, C2, C3, C4>::_value;
};

using Blue = Color<ColorComp::B<255>>;

static_assert(Blue::_r == 0);
static_assert(Blue::_g == 0);
static_assert(Blue::_b == 255);
static_assert(Blue::_a == 0);
