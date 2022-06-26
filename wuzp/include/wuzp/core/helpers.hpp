#pragma once

#include <array>
#include <string_view>
#include <tuple>
#include <typeinfo>

namespace wuzp {

template <typename T>
struct FailOnInstantiation : public std::false_type {};

template <int Size, typename T>
using HomogeneousTuple = decltype(std::tuple_cat(std::declval<std::array<T, Size>>()));

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

template <typename...>
using Void = void;

template <template <typename...> typename, typename...>
struct IsSpecializableImpl : std::false_type {};
template <template <typename...> typename C, typename... Ts>
struct IsSpecializableImpl<C, Void<C<Ts...>>, Ts...> : std::true_type {};
template <template <typename...> typename C, typename... Ts>
struct IsSpecializable : IsSpecializableImpl<C, void, Ts...> {};

template <typename T, template <typename...> typename Template>
struct IsSpecialization {
  static constexpr bool value = false;
};
template <template <typename...> typename Template1, template <typename...> typename Template2, typename... Args>
struct IsSpecialization<Template1<Args...>, Template2> {
  static constexpr bool calculate() {
    if constexpr (IsSpecializable<Template2, Args...>::value) {
      if constexpr (std::is_same_v<Template1<Args...>, Template2<Args...>>) {
        return true;
      }
    }
    return false;
  }
  static constexpr bool value = calculate();
};
// TODO(remod): This corner case could be avoided by using std::integral_constant instead of ints for the Tensor class and providing a
// simplified alias.
template <typename T, template <int, int, typename...> typename Template>
struct IsSpecializationInts : std::false_type {};
template <template <int, int, typename...> typename Template, int Int0, int Int1, typename... Args>
struct IsSpecializationInts<Template<Int0, Int1, Args...>, Template> : std::true_type {};

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

template <typename T>
struct SpecializationDummy;

template <typename T>
constexpr auto getTypeName() noexcept {
  std::string_view name;
  std::string_view prefix;
  std::string_view suffix;
#ifdef __clang__
  name = __PRETTY_FUNCTION__;
  prefix = "auto type_name() [T = ";
  suffix = "]";
#elif defined(__GNUC__)
  name = __PRETTY_FUNCTION__;
  prefix = "constexpr auto getTypeName() [with T = ";
  suffix = "]";
#elif defined(_MSC_VER)
  name = __FUNCSIG__;
  prefix = "auto __cdecl type_name<";
  suffix = ">(void) noexcept";
#endif
  name.remove_prefix(prefix.size());
  name.remove_suffix(suffix.size());
  return name;
}

}  // namespace wuzp
