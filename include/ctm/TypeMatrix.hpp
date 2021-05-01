#pragma once

#include <tuple>

#include "ctm/helpers.hpp"
#include "ctm/traits.hpp"

namespace ctm {

template <size_t Cols, typename Tuple, typename IndexSequence>
struct ToRowsImpl;

template <size_t Cols, typename... Ts, size_t... RowIs>
struct ToRowsImpl<Cols, std::tuple<Ts...>, std::index_sequence<RowIs...>> {
  using type =
      typename Cat<typename std::tuple<typename GetSubset<RowIs * Cols, RowIs * Cols + Cols, 1, std::tuple<Ts...>>::type>...>::type;
};

template <size_t Cols, typename... Ts>
using ToRows = ToRowsImpl<Cols, std::tuple<Ts...>, std::make_index_sequence<sizeof...(Ts) / Cols>>;

template <size_t Cols, typename Tuple, typename IndexSequence>
struct ToColsImpl;

template <size_t Cols, typename... Ts, size_t... ColIs>
struct ToColsImpl<Cols, std::tuple<Ts...>, std::index_sequence<ColIs...>> {
  using type = typename Cat<typename std::tuple<typename GetSubset<ColIs, sizeof...(Ts), Cols, std::tuple<Ts...>>::type>...>::type;
};

template <size_t Cols, typename... Ts>
using ToCols = ToColsImpl<Cols, std::tuple<Ts...>, std::make_index_sequence<Cols>>;

template <typename Lhs, typename Rhs, typename IndexSequence>
struct AddTwoTuplesImpl;

template <typename... TsLhs, typename... TsRhs, size_t... Is>
struct AddTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::index_sequence<Is...>> {
  using type = std::tuple<typename Sum<typename std::tuple_element<Is, std::tuple<TsLhs...>>::type,
                                       typename std::tuple_element<Is, std::tuple<TsRhs...>>::type>::type...>;
};

template <typename Lhs, typename Rhs>
struct AddTwoTuples;

template <typename... TsLhs, typename... TsRhs>
struct AddTwoTuples<std::tuple<TsLhs...>, std::tuple<TsRhs...>> {
  static_assert(sizeof...(TsLhs) == sizeof...(TsRhs), "Tuple size mismatch.");
  using type = typename AddTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::make_index_sequence<sizeof...(TsLhs)>>::type;
};

template <typename Lhs, typename Rhs, typename IndexSequence>
struct MultiplyTwoTuplesImpl;

template <typename... TsLhs, typename... TsRhs, size_t... Is>
struct MultiplyTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::index_sequence<Is...>> {
  using type = typename Sum<typename Product<typename std::tuple_element<Is, std::tuple<TsLhs...>>::type,
                                             typename std::tuple_element<Is, std::tuple<TsRhs...>>::type>::type...>::type;
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
  using type = std::tuple<
      typename MultiplyTwoTuples<typename std::tuple_element<Is / sizeof...(TuplesRhs), std::tuple<TuplesLhs...>>::type,
                                 typename std::tuple_element<Is % sizeof...(TuplesRhs), std::tuple<TuplesRhs...>>::type>::type...>;
};

template <int Rows, int Cols, typename Units>
struct TypeMatrixImpl;

template <int Rows, int Cols, typename... Units>
struct TypeMatrixImpl<Rows, Cols, std::tuple<Units...>> {
  static_assert(sizeof...(Units) == Rows * Cols, "Number of units does not match matrix size.");
  using AsTuple = std::tuple<Units...>;
};

template <int RowsLhs, int ColsLhs, typename... UnitsLhs, int RowsRhs, int ColsRhs, typename... UnitsRhs>
struct Sum<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<UnitsLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<UnitsRhs...>>> {
  static_assert(RowsLhs == RowsRhs, "Matrix size mismatch.");
  static_assert(ColsLhs == ColsRhs, "Matrix size mismatch.");
  using type = TypeMatrixImpl<RowsLhs, ColsRhs, typename AddTwoTuples<std::tuple<UnitsLhs...>, std::tuple<UnitsRhs...>>::type>;
};

template <int RowsLhs, int ColsLhs, typename... UnitsLhs, int RowsRhs, int ColsRhs, typename... UnitsRhs>
struct Product<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<UnitsLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<UnitsRhs...>>> {
  static_assert(ColsLhs == RowsRhs, "Matrix size mismatch.");
  using type =
      TypeMatrixImpl<RowsLhs, ColsRhs,
                     typename MultiplyTuples<typename ToRows<ColsLhs, UnitsLhs...>::type, typename ToCols<ColsRhs, UnitsRhs...>::type,
                                             std::make_index_sequence<RowsLhs * ColsRhs>>::type>;
};

template <int Rows, int Cols, typename... Units>
struct Transpose<TypeMatrixImpl<Rows, Cols, std::tuple<Units...>>> {
  using type = TypeMatrixImpl<Cols, Rows, typename ExtractAndApply<Cat, typename ToCols<Cols, Units...>::type>::type>;
};

template <int Rows, int Cols, typename... Units>
struct TypeMatrixHelper;

template <int Rows, int Cols, typename Unit0, typename... Units>
struct TypeMatrixHelper<Rows, Cols, Unit0, Units...> {
  static_assert(sizeof...(Units) + 1 == Rows * Cols, "Number of units does not match matrix size.");
  using type = TypeMatrixImpl<Rows, Cols, std::tuple<Unit0, Units...>>;
};

template <int Rows, int Cols, typename Unit>
struct TypeMatrixHelper<Rows, Cols, Unit> {
  using type = TypeMatrixImpl<Rows, Cols, HomogeneousTuple<Rows * Cols, Unit>>;
};

template <int Rows, int Cols, typename... Units>
using TypeMatrix = typename TypeMatrixHelper<Rows, Cols, Units...>::type;

}  // namespace ctm
