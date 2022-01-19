#pragma once

#include <tuple>

#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

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
  static_assert(sizeof...(TsLhs) == sizeof...(TsRhs), "Tuple sizes do not match.");
  using type = typename AddTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::make_index_sequence<sizeof...(TsLhs)>>::type;
};

template <typename Lhs, typename Rhs, typename IndexSequence>
struct SubtractTwoTuplesImpl;
template <typename... TsLhs, typename... TsRhs, size_t... Is>
struct SubtractTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::index_sequence<Is...>> {
  using type = std::tuple<typename Difference<typename std::tuple_element<Is, std::tuple<TsLhs...>>::type,
                                              typename std::tuple_element<Is, std::tuple<TsRhs...>>::type>::type...>;
};

template <typename Lhs, typename Rhs>
struct SubtractTwoTuples;
template <typename... TsLhs, typename... TsRhs>
struct SubtractTwoTuples<std::tuple<TsLhs...>, std::tuple<TsRhs...>> {
  static_assert(sizeof...(TsLhs) == sizeof...(TsRhs), "Tuple sizes do not match.");
  using type = typename SubtractTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::make_index_sequence<sizeof...(TsLhs)>>::type;
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

template <int Rows, int Cols, typename Types>
struct TypeMatrixImpl;
template <int Rows, int Cols, typename... Types>
struct TypeMatrixImpl<Rows, Cols, std::tuple<Types...>> {
  static_assert(sizeof...(Types) == Rows * Cols, "Number of types does not match matrix size.");
};

// TODO(remod): Can we simply remove the next two declarations?
template <int Rows, int Cols, typename... Types>
struct CreateTypeMatrix;
template <int Rows, int Cols>
struct CreateTypeMatrix<Rows, Cols> {
  static_assert(FailOnInstantiation<CreateTypeMatrix<Rows, Cols>>::value, "Empty type matrix is not allowed.");
};
template <int Rows, int Cols, typename Type0, typename... Types>
struct CreateTypeMatrix<Rows, Cols, Type0, Types...> {
  static_assert(sizeof...(Types) + 1 == Rows * Cols, "Number of types does not match matrix size.");
  using type = TypeMatrixImpl<Rows, Cols, std::tuple<Type0, Types...>>;
};
template <int Rows, int Cols, typename Type>
struct CreateTypeMatrix<Rows, Cols, Type> {
  using type = TypeMatrixImpl<Rows, Cols, HomogeneousTuple<Rows * Cols, Type>>;
};

template <typename T>
struct GetTypes;
template <int Rows, int Cols, typename Types>
struct GetTypes<TypeMatrixImpl<Rows, Cols, Types>> {
  using type = Types;
};

template <typename Types>
struct Dimension<TypeMatrixImpl<1, 1, Types>> {
  static constexpr int value = 0;
};
template <int _Rows, typename Types>
struct Dimension<TypeMatrixImpl<_Rows, 1, Types>> {
  static constexpr int value = 1;
};
template <int _Cols, typename Types>
struct Dimension<TypeMatrixImpl<1, _Cols, Types>> {
  static constexpr int value = 1;
};
template <int _Rows, int _Cols, typename Types>
struct Dimension<TypeMatrixImpl<_Rows, _Cols, Types>> {
  static constexpr int value = 2;
};

template <int _Rows, int _Cols, typename Types>
struct Rows<TypeMatrixImpl<_Rows, _Cols, Types>> {
  static constexpr int value = _Rows;
};

template <int _Rows, int _Cols, typename Types>
struct Cols<TypeMatrixImpl<_Rows, _Cols, Types>> {
  static constexpr int value = _Cols;
};

template <int Rows, int Cols, typename... Types, int ElementRowId, int ElementColId>
struct Element<TypeMatrixImpl<Rows, Cols, std::tuple<Types...>>, ElementRowId, ElementColId> {
  static_assert(ElementRowId >= 0 && ElementRowId < Rows, "Element row id is out of range.");
  static_assert(ElementColId >= 0 && ElementColId < Cols, "Element columns id is out of range.");
  using type = typename std::tuple_element<ElementRowId * Cols + ElementColId, std::tuple<Types...>>::type;
};

template <int Cols, int BlockRowId, int BlockColId, int BlockRows, int BlockCols, typename Tuple, typename IndexSequence>
struct BlockImpl;
template <int Cols, int BlockRowId, int BlockColId, int BlockRows, int BlockCols, typename... Ts, size_t... BlockRowIs>
struct BlockImpl<Cols, BlockRowId, BlockColId, BlockRows, BlockCols, std::tuple<Ts...>, std::index_sequence<BlockRowIs...>> {
  using type =
      typename Cat<typename GetSubset<(BlockRowId + BlockRowIs) * Cols + BlockColId,
                                      (BlockRowId + BlockRowIs) * Cols + BlockColId + BlockCols, 1, std::tuple<Ts...>>::type...>::type;
};

template <int Rows, int Cols, typename... Types, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<TypeMatrixImpl<Rows, Cols, std::tuple<Types...>>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  static_assert(BlockRowId >= 0 && BlockColId >= 0, "Block top left corner is out of range.");
  static_assert(BlockRowId + BlockRows <= Rows && BlockColId + BlockCols <= Cols, "Block bottom right corner is out of range.");
  using type = TypeMatrixImpl<BlockRows, BlockCols,
                              typename BlockImpl<Cols, BlockRowId, BlockColId, BlockRows, BlockCols, std::tuple<Types...>,
                                                 std::make_index_sequence<BlockRows>>::type>;
};

template <int RowsLhs, int ColsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct Sum<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhs == RowsRhs, "Matrix sizes do not match.");
  static_assert(ColsLhs == ColsRhs, "Matrix sizes do not match.");
  using type = TypeMatrixImpl<RowsLhs, ColsRhs, typename AddTwoTuples<std::tuple<TypesLhs...>, std::tuple<TypesRhs...>>::type>;
};

template <int RowsLhs, int ColsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct Difference<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhs == RowsRhs, "Matrix sizes do not match.");
  static_assert(ColsLhs == ColsRhs, "Matrix sizes do not match.");
  using type = TypeMatrixImpl<RowsLhs, ColsRhs, typename SubtractTwoTuples<std::tuple<TypesLhs...>, std::tuple<TypesRhs...>>::type>;
};

// So far, we do not require the RHS traits.
template <int RowsLhs, int ColsLhs, typename... TypesLhs, typename TypeRhs>
struct Product<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeRhs> {
  using type = TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<typename Product<TypesLhs, TypeRhs>::type...>>;
};
// So far, we do not require the LHS traits.
template <typename TypeLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct Product<TypeLhs, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  using type = typename Product<TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>, TypeLhs>::type;
};
template <int RowsLhs, int ColsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct Product<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  static_assert(ColsLhs == RowsRhs, "Matrix sizes do not match.");
  using type =
      TypeMatrixImpl<RowsLhs, ColsRhs,
                     typename MultiplyTuples<typename ToRows<ColsLhs, TypesLhs...>::type, typename ToCols<ColsRhs, TypesRhs...>::type,
                                             std::make_index_sequence<RowsLhs * ColsRhs>>::type>;
};

// So far, we do not require the RHS traits.
template <int RowsLhs, int ColsLhs, typename... TypesLhs, typename TypeRhs>
struct Division<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeRhs> {
  using type = TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<typename Division<TypesLhs, TypeRhs>::type...>>;
};

template <int Rows, int Cols, typename... Types>
struct Transpose<TypeMatrixImpl<Rows, Cols, std::tuple<Types...>>> {
  using type = TypeMatrixImpl<Cols, Rows, typename ExtractAndApply<Cat, typename ToCols<Cols, Types...>::type>::type>;
};

template <int RowsLhs, int ColsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct DotProduct<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhs == RowsRhs, "Matrix sizes do not match.");
  static_assert(ColsLhs == 1, "Dot product is only defined for vectors.");
  static_assert(ColsRhs == 1, "Dot product is only defined for vectors.");
  using type = typename Element<typename Product<typename Transpose<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>>::type,
                                                 TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>>::type,
                                0, 0>::type;
};

template <int Size, typename ZeroType, typename OneType, typename IndexSequence>
struct DiagonalTypeMatrixImpl;
template <int Size, typename ZeroType, typename OneType, size_t... Is>
struct DiagonalTypeMatrixImpl<Size, ZeroType, OneType, std::index_sequence<Is...>> {
  using type = TypeMatrixImpl<Size, Size, std::tuple<typename IfThenElse<Is % (Size + 1) != 0, ZeroType, OneType>::type...>>;
};

template <int Size, typename ZeroType, typename OneType>
using DiagonalTypeMatrix = typename DiagonalTypeMatrixImpl<Size, ZeroType, OneType, std::make_index_sequence<Size * Size>>::type;

// template <typename Matrix, typename Result, int Row, int Col>
// struct GaussianElimination;
// template <int Rows, int Cols, typename... Types, typename Result, int Row, int Col>
// struct GaussianElimination<TypeMatrixImpl<Rows, Cols, std::tuple<Types...>>, Result, Row, Col> {
//
//};
//
// template <int Rows, int Cols, typename... Types>
// struct Inverse<TypeMatrixImpl<Rows, Cols, std::tuple<Types...>>> {
//  static_assert(Rows == Cols, "Matrix is not square.");
//  using type = typename GaussianElimination<TypeMatrixImpl<Rows, Cols, std::tuple<Types...>>, typename Traits::template
//  Identity<TypeMatrixImpl<Rows, Cols, std::tuple<Types...>>>::type, Rows - 1, 0>::type;
//};

template <int Rows, int Cols, typename... Types>
using TypeMatrix = typename CreateTypeMatrix<Rows, Cols, Types...>::type;

template <int Rows, typename... Types>
using TypeVector = typename CreateTypeMatrix<Rows, 1, Types...>::type;

}  // namespace wuzp
