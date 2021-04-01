#pragma once

#include <array>
#include <string_view>
#include <tuple>
#include <typeinfo>

namespace ctm {

template <typename T>
struct FakeDependency : public std::false_type {};

template <int Size, typename T>
using ToTuple = decltype(std::tuple_cat(std::declval<std::array<T, Size>>()));

template <bool>
struct SelectIf {
  template <typename T>
  using type = std::tuple<T>;
};

template <>
struct SelectIf<false> {
  template <class T>
  using type = std::tuple<>;
};

template <int Start, int End, int Increment, typename Tuple, typename IndexSequence>
struct GetSubsetImpl;

template <int Start, int End, int Increment, typename... Ts, size_t... Is>
struct GetSubsetImpl<Start, End, Increment, std::tuple<Ts...>, std::index_sequence<Is...>> {
  using type = decltype(std::tuple_cat(
      std::declval<typename SelectIf<(Is >= Start) && (Is < End) && ((Is - Start) % Increment == 0)>::template type<Ts>>()...));
};

template <int Start, int End, int Increment, typename Tuple>
using GetSubset = GetSubsetImpl<Start, End, Increment, Tuple, std::make_index_sequence<std::tuple_size<Tuple>::value>>;

template <class T, class... Ts>
inline constexpr bool areSame = std::conjunction_v<std::is_same<T, Ts>...>;

template <typename T>
constexpr auto getTypeName() noexcept {
  std::string_view name, prefix, suffix;
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

}  // namespace ctm
