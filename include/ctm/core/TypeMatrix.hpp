#pragma once

#include <tuple>

#include "ctm/core/helpers.hpp"
#include "ctm/core/traits.hpp"

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

template <typename Traits, typename Lhs, typename Rhs, typename IndexSequence>
struct AddTwoTuplesImpl;
template <typename Traits, typename... TsLhs, typename... TsRhs, size_t... Is>
struct AddTwoTuplesImpl<Traits, std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::index_sequence<Is...>> {
  using type = std::tuple<typename Traits::template Sum<typename std::tuple_element<Is, std::tuple<TsLhs...>>::type,
                                                        typename std::tuple_element<Is, std::tuple<TsRhs...>>::type>::type...>;
};

template <typename Traits, typename Lhs, typename Rhs>
struct AddTwoTuples;
template <typename Traits, typename... TsLhs, typename... TsRhs>
struct AddTwoTuples<Traits, std::tuple<TsLhs...>, std::tuple<TsRhs...>> {
  static_assert(sizeof...(TsLhs) == sizeof...(TsRhs), "Tuple size mismatch.");
  using type =
      typename AddTwoTuplesImpl<Traits, std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::make_index_sequence<sizeof...(TsLhs)>>::type;
};

template <typename Traits, typename Lhs, typename Rhs, typename IndexSequence>
struct SubtractTwoTuplesImpl;
template <typename Traits, typename... TsLhs, typename... TsRhs, size_t... Is>
struct SubtractTwoTuplesImpl<Traits, std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::index_sequence<Is...>> {
  using type = std::tuple<typename Traits::template Difference<typename std::tuple_element<Is, std::tuple<TsLhs...>>::type,
                                                               typename std::tuple_element<Is, std::tuple<TsRhs...>>::type>::type...>;
};

template <typename Traits, typename Lhs, typename Rhs>
struct SubtractTwoTuples;
template <typename Traits, typename... TsLhs, typename... TsRhs>
struct SubtractTwoTuples<Traits, std::tuple<TsLhs...>, std::tuple<TsRhs...>> {
  static_assert(sizeof...(TsLhs) == sizeof...(TsRhs), "Tuple size mismatch.");
  using type =
      typename SubtractTwoTuplesImpl<Traits, std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::make_index_sequence<sizeof...(TsLhs)>>::type;
};

template <typename Traits, typename Lhs, typename Rhs, typename IndexSequence>
struct MultiplyTwoTuplesImpl;
template <typename Traits, typename... TsLhs, typename... TsRhs, size_t... Is>
struct MultiplyTwoTuplesImpl<Traits, std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::index_sequence<Is...>> {
  using type = typename Traits::template Sum<
      typename Traits::template Product<typename std::tuple_element<Is, std::tuple<TsLhs...>>::type,
                                        typename std::tuple_element<Is, std::tuple<TsRhs...>>::type>::type...>::type;
};

template <typename Traits, typename Lhs, typename Rhs>
struct MultiplyTwoTuples;
template <typename Traits, typename... TsLhs, typename... TsRhs>
struct MultiplyTwoTuples<Traits, std::tuple<TsLhs...>, std::tuple<TsRhs...>> {
  using type =
      typename MultiplyTwoTuplesImpl<Traits, std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::make_index_sequence<sizeof...(TsLhs)>>::type;
};

template <typename Traits, typename Lhs, typename Rhs, typename IndexSequence>
struct MultiplyTuples;
template <typename Traits, typename... TuplesLhs, typename... TuplesRhs, size_t... Is>
struct MultiplyTuples<Traits, std::tuple<TuplesLhs...>, std::tuple<TuplesRhs...>, std::index_sequence<Is...>> {
  using type = std::tuple<
      typename MultiplyTwoTuples<Traits, typename std::tuple_element<Is / sizeof...(TuplesRhs), std::tuple<TuplesLhs...>>::type,
                                 typename std::tuple_element<Is % sizeof...(TuplesRhs), std::tuple<TuplesRhs...>>::type>::type...>;
};

template <int Rows, int Cols, typename Traits, typename Types>
struct TypeMatrixImpl;
template <int Rows, int Cols, typename Traits, typename... Types>
struct TypeMatrixImpl<Rows, Cols, Traits, std::tuple<Types...>> {
  static_assert(sizeof...(Types) == Rows * Cols, "Number of types does not match matrix size.");
};

template <typename T>
struct AsTuple;
template <int Rows, int Cols, typename Traits, typename Types>
struct AsTuple<TypeMatrixImpl<Rows, Cols, Traits, Types>> {
  using type = Types;
};

template <int Rows, int Cols, typename Traits, typename... Types, int ElementRowId, int ElementColId>
struct Element<TypeMatrixImpl<Rows, Cols, Traits, std::tuple<Types...>>, ElementRowId, ElementColId> {
  static_assert(ElementRowId >= 0 && ElementColId >= 0, "Element ids are out of range.");
  static_assert(ElementRowId < Rows && ElementColId < Cols, "Element ids are out of range.");
  using type = typename std::tuple_element<ElementRowId * Cols + ElementColId - 1, std::tuple<Types...>>::type;
};

template <int Cols, int BlockRowId, int BlockColId, int BlockRows, int BlockCols, typename Tuple, typename IndexSequence>
struct BlockImpl;
template <int Cols, int BlockRowId, int BlockColId, int BlockRows, int BlockCols, typename... Ts, size_t... BlockRowIs>
struct BlockImpl<Cols, BlockRowId, BlockColId, BlockRows, BlockCols, std::tuple<Ts...>, std::index_sequence<BlockRowIs...>> {
  using type =
      typename Cat<typename GetSubset<(BlockRowId + BlockRowIs) * Cols + BlockColId,
                                      (BlockRowId + BlockRowIs) * Cols + BlockColId + BlockCols, 1, std::tuple<Ts...>>::type...>::type;
};

template <int Rows, int Cols, typename Traits, typename... Types, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<TypeMatrixImpl<Rows, Cols, Traits, std::tuple<Types...>>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  static_assert(BlockRowId >= 0 && BlockColId >= 0, "Block top left corner is out of range.");
  static_assert(BlockRowId + BlockRows <= Rows && BlockColId + BlockCols <= Cols, "Block bottom right corner is out of range.");
  using type = TypeMatrixImpl<BlockRows, BlockCols, Traits,
                              typename BlockImpl<Cols, BlockRowId, BlockColId, BlockRows, BlockCols, std::tuple<Types...>,
                                                 std::make_index_sequence<BlockRows>>::type>;
};

template <int RowsLhs, int ColsLhs, typename TraitsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename TraitsRhs,
          typename... TypesRhs>
struct Sum<TypeMatrixImpl<RowsLhs, ColsLhs, TraitsLhs, std::tuple<TypesLhs...>>,
           TypeMatrixImpl<RowsRhs, ColsRhs, TraitsRhs, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhs == RowsRhs, "Matrix size mismatch.");
  static_assert(ColsLhs == ColsRhs, "Matrix size mismatch.");
  static_assert(std::is_same<TraitsLhs, TraitsRhs>::value, "Traits mismatch.");
  using type =
      TypeMatrixImpl<RowsLhs, ColsRhs, TraitsLhs, typename AddTwoTuples<TraitsLhs, std::tuple<TypesLhs...>, std::tuple<TypesRhs...>>::type>;
};

template <int RowsLhs, int ColsLhs, typename TraitsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename TraitsRhs,
          typename... TypesRhs>
struct Difference<TypeMatrixImpl<RowsLhs, ColsLhs, TraitsLhs, std::tuple<TypesLhs...>>,
                  TypeMatrixImpl<RowsRhs, ColsRhs, TraitsRhs, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhs == RowsRhs, "Matrix size mismatch.");
  static_assert(ColsLhs == ColsRhs, "Matrix size mismatch.");
  static_assert(std::is_same<TraitsLhs, TraitsRhs>::value, "Traits mismatch.");
  using type = TypeMatrixImpl<RowsLhs, ColsRhs, TraitsLhs,
                              typename SubtractTwoTuples<TraitsLhs, std::tuple<TypesLhs...>, std::tuple<TypesRhs...>>::type>;
};

template <int RowsLhs, int ColsLhs, typename TraitsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename TraitsRhs,
          typename... TypesRhs>
struct Product<TypeMatrixImpl<RowsLhs, ColsLhs, TraitsLhs, std::tuple<TypesLhs...>>,
               TypeMatrixImpl<RowsRhs, ColsRhs, TraitsRhs, std::tuple<TypesRhs...>>> {
  static_assert(ColsLhs == RowsRhs, "Matrix size mismatch.");
  static_assert(std::is_same<TraitsLhs, TraitsRhs>::value, "Traits mismatch.");
  using type = TypeMatrixImpl<
      RowsLhs, ColsRhs, TraitsLhs,
      typename MultiplyTuples<TraitsLhs, typename ToRows<ColsLhs, TypesLhs...>::type, typename ToCols<ColsRhs, TypesRhs...>::type,
                              std::make_index_sequence<RowsLhs * ColsRhs>>::type>;
};

template <int Rows, int Cols, typename Traits, typename... Types>
struct Transpose<TypeMatrixImpl<Rows, Cols, Traits, std::tuple<Types...>>> {
  using type = TypeMatrixImpl<Cols, Rows, Traits, typename ExtractAndApply<Cat, typename ToCols<Cols, Types...>::type>::type>;
};

template <int RowsLhs, int ColsLhs, typename TraitsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename TraitsRhs,
          typename... TypesRhs>
struct DotProduct<TypeMatrixImpl<RowsLhs, ColsLhs, TraitsLhs, std::tuple<TypesLhs...>>,
                  TypeMatrixImpl<RowsRhs, ColsRhs, TraitsRhs, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhs == RowsRhs, "Matrix size mismatch.");
  static_assert(ColsLhs == 1, "Dot product is only defined for vectors.");
  static_assert(ColsRhs == 1, "Dot product is only defined for vectors.");
  static_assert(std::is_same<TraitsLhs, TraitsRhs>::value, "Traits mismatch.");
  using type = typename Product<typename Transpose<TypeMatrixImpl<RowsLhs, ColsLhs, TraitsLhs, std::tuple<TypesLhs...>>>::type,
                                TypeMatrixImpl<RowsRhs, ColsRhs, TraitsRhs, std::tuple<TypesRhs...>>>::type;
};

// template <int Size, typename ZeroType, typename OneType, typename IndexSequence>
// struct IdentityImpl;
//
// template <int Size, typename ZeroType, typename OneType, size_t... Is>
// struct IdentityImpl<Size, ZeroType, OneType, std::index_sequence<Is...>> {
//  using type = TypeMatrixImpl<Size, Size, std::tuple<typename Chose<Is % (Size + 1) != 0, ZeroType, OneType>::type...>>;
//};
//
// template <int Size, typename ZeroType, typename OneType>
// using Identity = typename IdentityImpl<Size, ZeroType, OneType, std::make_index_sequence<Size * Size>>::type;

template <int Size, typename Traits, typename TypeOfFamily>
using TypeMatrixIdentity =
    TypeMatrixImpl<Size, Size, Traits, HomogeneousTuple<Size * Size, typename Traits::template Identity<TypeOfFamily>::type>>;

template <int Rows, int Cols, typename Traits, typename... Types>
struct Inverse<TypeMatrixImpl<Rows, Cols, Traits, std::tuple<Types...>>> {
  static_assert(Rows == Cols, "Matrix is not square.");
  // Invert the types individually to get a candidate for the result of the inversion.
  using type = TypeMatrixImpl<Rows, Cols, Traits, std::tuple<typename Traits::template Inverse<Types>::type...>>;
  // Verify the candidate by multiplying it with the original matrix.
  static_assert(std::is_same<typename Product<TypeMatrixImpl<Rows, Cols, Traits, std::tuple<Types...>>, type>::type,
                             TypeMatrixIdentity<Rows, Traits, typename std::tuple_element<0, std::tuple<Types...>>::type>>::value,
                "Matrix is not invertible.");
};

template <int Rows, int Cols, typename Traits, typename... Types>
struct TypeMatrixInterface;
template <int Rows, int Cols, typename Traits>
struct TypeMatrixInterface<Rows, Cols, Traits> {
  static_assert(FakeDependency<TypeMatrixInterface<Rows, Cols, Traits>>::value, "Empty type matrix is not allowed.");
};
template <int Rows, int Cols, typename Traits, typename Type0, typename... Types>
struct TypeMatrixInterface<Rows, Cols, Traits, Type0, Types...> {
  static_assert(sizeof...(Types) + 1 == Rows * Cols, "Number of types does not match matrix size.");
  using type = TypeMatrixImpl<Rows, Cols, Traits, std::tuple<Type0, Types...>>;
};
template <int Rows, int Cols, typename Traits, typename Type>
struct TypeMatrixInterface<Rows, Cols, Traits, Type> {
  using type = TypeMatrixImpl<Rows, Cols, Traits, HomogeneousTuple<Rows * Cols, Type>>;
};

template <int Rows, int Cols, typename Traits, typename... Types>
using TypeMatrix = typename TypeMatrixInterface<Rows, Cols, Traits, Types...>::type;

}  // namespace ctm
