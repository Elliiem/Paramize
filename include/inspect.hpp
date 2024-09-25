#pragma once

#include "parameter.hpp"
#include "parse_parameters.hpp"
//  #include "parameter.hpp"
//      #include "tag.hpp"
//          #include "type_stack.hpp"

// namespace Paramize {

// get nth param

namespace Set {

template<AnySetParamRegion region, AnyTaggedParamStack<typename region::_tagset> params, AnyParamTag tag>
struct GetParamHelper;

// get param (set)
template<AnyParseRegionOkayResult parsed, MemberTag<typename parsed::_region::_tagset> tag>
using getParam = Null;

} // namespace Set

// namespace List {

// get index of nth instance
// template<AnyParseRegionOkayResult parsed, MemberTag<typename parsed::_region::_tagset> tag>
// using findListRegionParam = Null;

// } // namespace List

// TODO:
// find regions based on an identifier
// get nth region

// TODO:
// get nth instance of param (global)
// get number of instances (global u list)

// } // namespace Paramize
