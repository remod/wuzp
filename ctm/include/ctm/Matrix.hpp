#include <iostream>
#include <typeinfo>
#include <string_view>
#include <tuple>

#include <boost/units/systems/si.hpp>

#include <Eigen/Core>

using namespace boost::units;
using namespace boost::units::si;

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

template<int Start, int End, int Increment, typename Tuple, typename IndexSequence>
struct GNthElementsImpl;

template<int Start, int End, int Increment, typename... Ts, size_t... Is>
struct GNthElementsImpl<Start, End, Increment, std::tuple<Ts...>, std::index_sequence<Is...>> {
    using type = decltype(std::tuple_cat(
        std::declval<typename SelectIf<(Is >= Start) && (Is < End) && ((Is - Start) % Increment == 0)>::template type<Ts>>()...
    ));
};

template<int Start, int End, int Increment, typename Tuple>
using GNthElements = GNthElementsImpl<Start, End, Increment, Tuple, std::make_index_sequence<std::tuple_size<Tuple>::value>>;


template<size_t Cols, typename Tuple, typename IndexSequence>
struct ToRowsImpl;

template <size_t Cols, typename... Ts, size_t... RowIs>
struct ToRowsImpl<Cols, std::tuple<Ts...>, std::index_sequence<RowIs...>> {
  using type = decltype(std::tuple_cat(
      std::declval<typename std::tuple<typename GNthElements<RowIs * Cols, RowIs * Cols + Cols, 1, std::tuple<Ts...>>::type>>()...
  ));
};

template<size_t Cols, typename... Ts>
using ToRows = ToRowsImpl<Cols, std::tuple<Ts...>, std::make_index_sequence<sizeof...(Ts) / Cols>>;

template<size_t Cols, typename Tuple, typename IndexSequence>
struct ToColsImpl;

template <size_t Cols, typename... Ts, size_t... ColIs>
struct ToColsImpl<Cols, std::tuple<Ts...>, std::index_sequence<ColIs...>> {
  using type = decltype(std::tuple_cat(
      std::declval<typename std::tuple<typename GNthElements<ColIs, sizeof...(Ts), Cols, std::tuple<Ts...>>::type>>()...
  ));
};

template<size_t Cols, typename... Ts>
using ToCols = ToColsImpl<Cols, std::tuple<Ts...>, std::make_index_sequence<Cols>>;


template <class T, class... Ts>
inline constexpr bool areSame = std::conjunction_v<std::is_same<T, Ts>...>;

template <typename U1, typename U2>
using add = typename add_typeof_helper<U1, U2>::type;

template <typename U1, typename U2>
using multiply = typename multiply_typeof_helper<U1, U2>::type;

template <typename Lhs, typename Rhs, typename IndexSequence>
struct MultiplyTwoTuplesImpl;

template <typename... TsLhs, typename... TsRhs, size_t... Is>
struct MultiplyTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::index_sequence<Is...>> {
  static_assert(areSame<multiply<typename std::tuple_element<Is, std::tuple<TsLhs...>>::type, typename std::tuple_element<Is, std::tuple<TsRhs...>>::type>...>);
  using type = multiply<typename std::tuple_element<0, std::tuple<TsLhs...>>::type, typename std::tuple_element<0, std::tuple<TsRhs...>>::type>;
};

template <typename Lhs, typename Rhs>
struct MultiplyTwoTuples;

template <typename... TsLhs, typename... TsRhs>
struct MultiplyTwoTuples<std::tuple<TsLhs...>, std::tuple<TsRhs...>> {
  using type = typename MultiplyTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::make_index_sequence<sizeof...(TsLhs)>>::type;
};

template <typename Lhs, typename Rhs, typename IndexSequence>
struct MultiplyTuples;

template <typename... TuplesLhs, typename... TuplesRhs, size_t... Is>
struct MultiplyTuples<std::tuple<TuplesLhs...>, std::tuple<TuplesRhs...>, std::index_sequence<Is...>> {
  using type = std::tuple<typename MultiplyTwoTuples<typename std::tuple_element<Is / sizeof...(TuplesRhs), std::tuple<TuplesLhs...>>::type, typename std::tuple_element<Is % sizeof...(TuplesRhs), std::tuple<TuplesRhs...>>::type>::type...>;
};

template <typename U1, typename U2>
using divide = typename divide_typeof_helper<U1, U2>::type;



template <typename Lhs, typename Rhs>
struct Multiply;


template <int Rows, int Cols, typename... Units>
class Matrix {
  static_assert(sizeof...(Units) == Rows * Cols);
  
 public:
  static constexpr bool isHomogeneous = areSame<Units...>;
  
  Matrix<Rows, Cols, Units...> operator+(const Matrix<Rows, Cols, Units...>& rhs) const {
    std::cout << "operator+()" << std::endl;
    return Matrix<Rows, Cols, Units...>();
  }
  
  template <typename UnitRhs>
  Matrix<Rows, Cols, divide<Units, UnitRhs>...> operator/(quantity<UnitRhs> quantity) const {
    std::cout << "operator/()" << std::endl;
    return Matrix<Rows, Cols, divide<Units, UnitRhs>...>();
  }
  
  template <bool isHomogeneousCopy = isHomogeneous>
  std::enable_if_t<isHomogeneousCopy, Matrix<Rows, Cols, divide<dimensionless, Units>...>> inverse() const {
    std::cout << "inverse()" << std::endl;
    return Matrix<Rows, Cols, divide<dimensionless, Units>...>();
  }
};

template <int Rows, int Cols, typename Units>
struct ToMatrix;

template <int Rows, int Cols, typename... Units>
struct ToMatrix<Rows, Cols, std::tuple<Units...>> {
  using type = Matrix<Rows, Cols, Units...>;
};

template <int RowsLhs, int ColsLhs, typename... UnitsLhs, int ColsRhs, typename... UnitsRhs>
struct Multiply<Matrix<RowsLhs, ColsLhs, UnitsLhs...>, Matrix<ColsLhs, ColsRhs, UnitsRhs...>> {
  using type = typename ToMatrix<RowsLhs, ColsRhs, typename MultiplyTuples<typename ToRows<ColsLhs, UnitsLhs...>::type, typename ToCols<ColsRhs, UnitsRhs...>::type, std::make_index_sequence<RowsLhs * ColsRhs>>::type>::type;
};
  
template <int RowsLhs, int ColsLhs, typename... UnitsLhs, int ColsRhs, typename... UnitsRhs>
typename Multiply<Matrix<RowsLhs, ColsLhs, UnitsLhs...>, Matrix<ColsLhs, ColsRhs, UnitsRhs...>>::type operator*(const Matrix<RowsLhs, ColsLhs, UnitsLhs...>& lhs, const Matrix<ColsLhs, ColsRhs, UnitsRhs...>& rhs) {
  std::cout << "operator*()" << std::endl;
  return typename Multiply<Matrix<RowsLhs, ColsLhs, UnitsLhs...>, Matrix<ColsLhs, ColsRhs, UnitsRhs...>>::type();
}

template <typename T>
constexpr auto type_name() noexcept {
  std::string_view name, prefix, suffix;
#ifdef __clang__
  name = __PRETTY_FUNCTION__;
  prefix = "auto type_name() [T = ";
  suffix = "]";
#elif defined(__GNUC__)
  name = __PRETTY_FUNCTION__;
  prefix = "constexpr auto type_name() [with T = ";
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

