#pragma once

#include "Transformation.hpp"

namespace ctm {

template <typename Derived, typename From, typename To>
class TranslationBase : public PassiveTransformation<Derived, From, To> {};

}  // namespace ctm
