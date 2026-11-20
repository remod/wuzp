#pragma once

// Numerical derivatives of functions of time, against which the tests check analytic derivatives, e.g. of rotations and of the
// motion of rigid bodies.

//! The central difference (f(h) - f(-h)) / 2h of a function f of the time relative to some instant, e.g. of the quantities of a
//! rotation or of a point, for the time step h, a time quantity. It is accurate to the second order in h.
template <typename TimeT_, typename FunctionT_>
auto centralDifference(const TimeT_& step, const FunctionT_& function) {
  return (function(step) - function(step * -1.0)) / (step * 2.0);
}

//! The second central difference ((f(h) - f(0)) - (f(0) - f(-h))) / h^2 of a function f of the time, e.g. the acceleration of a
//! point, whose differences are vectors. It is accurate to the second order in the time step h.
template <typename TimeT_, typename FunctionT_>
auto secondCentralDifference(const TimeT_& step, const FunctionT_& function) {
  const auto middle = function(step * 0.0);
  return ((function(step) - middle) - (middle - function(step * -1.0))) / (step * step);
}
