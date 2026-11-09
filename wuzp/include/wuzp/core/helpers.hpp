#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace wuzp {

template <typename T>
struct FailOnInstantiation : public std::false_type {};

template <int Size, typename T>
using HomogeneousTuple = decltype(std::tuple_cat(std::declval<std::array<T, std::size_t{Size}>>()));

template <typename... Tuples>
struct Cat {
  using type = decltype(std::tuple_cat(std::declval<Tuples>()...));
};

template <template <typename...> typename X, typename T>
struct ExtractAndApply;
template <template <typename...> typename X, typename... Ts>
struct ExtractAndApply<X, std::tuple<Ts...>> {
  using type = typename X<Ts...>::type;
};

template <bool>
struct SelectIf {
  template <typename T>
  using type = std::tuple<T>;
};
template <>
struct SelectIf<false> {
  template <typename T>
  using type = std::tuple<>;
};

//! Trait to select one type or another depending on a boolean. Note that both types must be defined even if not selected.
template <bool ChooseTypeA, typename TypeA, typename TypeB>
struct IfThenElse;
template <typename TypeA, typename TypeB>
struct IfThenElse<true, TypeA, TypeB> {
  using type = TypeA;
};
template <typename TypeA, typename TypeB>
struct IfThenElse<false, TypeA, TypeB> {
  using type = TypeB;
};

//! Whether the template can be specialized with the arguments, e.g. whether their number fits and they satisfy its constraints.
template <template <typename...> typename Template, typename... Args>
concept Specializable = requires { typename Template<Args...>; };

//! Whether constructing a Class from the arguments copies or moves it, i.e. whether they are a single Class. Forwarding
//! constructors exclude these arguments, so that they do not hijack the copy and move constructors: for a non-const lvalue,
//! a forwarding reference is a better match than the const reference of the copy constructor.
template <typename Class, typename... Args>
concept CopiesOrMoves = sizeof...(Args) == 1 && (std::same_as<std::remove_cvref_t<Args>, Class> && ...);

template <typename T, template <typename...> typename Template>
struct IsSpecialization : std::false_type {};
// Template2<Args...> is only formed if it is valid.
template <template <typename...> typename Template1, template <typename...> typename Template2, typename... Args>
  requires Specializable<Template2, Args...>
struct IsSpecialization<Template1<Args...>, Template2> : std::is_same<Template1<Args...>, Template2<Args...>> {};
template <int Start, int End, int Increment, typename Tuple, typename IndexSequence>
struct GetSubsetImpl;
template <int Start, int End, int Increment, typename... Ts, size_t... Is>
struct GetSubsetImpl<Start, End, Increment, std::tuple<Ts...>, std::index_sequence<Is...>> {
  using type = typename Cat<typename SelectIf<(Is >= Start) && (Is < End) && ((Is - Start) % Increment == 0)>::template type<Ts>...>::type;
};

template <int Start, int End, int Increment, typename Tuple>
using GetSubset = GetSubsetImpl<Start, End, Increment, Tuple, std::make_index_sequence<std::tuple_size<Tuple>::value>>;

template <typename T, typename... Ts>
inline constexpr bool areSame = std::conjunction_v<std::is_same<T, Ts>...>;

template <typename... Ts>
struct IsHomogeneous {
  static constexpr bool value = areSame<Ts...>;
};
template <typename... Ts>
struct IsHomogeneous<std::tuple<Ts...>> {
  static constexpr bool value = IsHomogeneous<Ts...>::value;
};

// TODO(remod): Use "value" or "value" everywhere for consistency?
//! Wrapper for partial specializations of trait for values types.
template <typename T>
struct ValueType;

//! Type wrapper which is used to access traits partially specialized in the implementation header files.
template <typename Implementation>
struct AccessImplementation;

//! Applies the comma operator from left to right: applyCommas(a, b, c) evaluates ((a, b), c). The backends use it for the
//! comma initializers of their libraries, e.g. (matrix << 1), 2, 3 with Eigen. All temporaries live until the end of the
//! expression.
template <typename First, typename... Rest>
void applyCommas(First&& first, Rest&&... rest) {
  static_cast<void>((std::forward<First>(first), ..., std::forward<Rest>(rest)));
}

}  // namespace wuzp
