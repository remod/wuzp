#pragma once

// The types of the wuzp benchmarks (wuzp.cpp) and of the comparison of the generated code (codegen/wuzp.cpp). Unlike types in an
// anonymous namespace, they have external linkage, so that the functions of codegen/wuzp.cpp that take them do too, and the
// compiler generates these functions on their own.

#include <wuzp/wuzp.hpp>
#include <wuzp_units/Unit.hpp>

#include "value_matrix.hpp"

namespace wuzp_benchmarks {

struct A {};
struct B {};
struct C {};
using CoordinateSystemA = wuzp::CoordinateSystem<A>;
using CoordinateSystemB = wuzp::CoordinateSystem<B>;
using CoordinateSystemC = wuzp::CoordinateSystem<C>;

template <typename CoordinateSystemT_>
using Velocity = wuzp::VelocityVector3<ValueMatrix, double, CoordinateSystemT_>;
template <typename CoordinateSystemT_>
using Position = wuzp::PositionPoint3<ValueMatrix, double, CoordinateSystemT_>;
using Acceleration = wuzp::AccelerationVector3<ValueMatrix, double, CoordinateSystemA>;
using Force = wuzp::ForceVector3<ValueMatrix, double, CoordinateSystemA>;
using Mass = QuantityD<wuzp_units::Mass>;
using Speed = QuantityD<wuzp_units::Velocity>;
template <typename FromT_, typename ToT_>
using Transformation = wuzp::PassiveProperRigidTransformation3<ValueMatrix, FromT_, ToT_, double>;

}  // namespace wuzp_benchmarks
