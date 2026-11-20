#pragma once

#include <algorithm>
#include <cstddef>
#include <tuple>
#include <utility>

#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <size_t ColsV_, typename Tuple, typename IndexSequence>
struct ToRowsImpl;
template <size_t ColsV_, typename... Ts, size_t... RowIs>
struct ToRowsImpl<ColsV_, std::tuple<Ts...>, std::index_sequence<RowIs...>> {
  using type =
      typename Cat<typename std::tuple<typename GetSubset<RowIs * ColsV_, RowIs * ColsV_ + ColsV_, 1, std::tuple<Ts...>>::type>...>::type;
};

template <size_t ColsV_, typename... Ts>
using ToRows = ToRowsImpl<ColsV_, std::tuple<Ts...>, std::make_index_sequence<sizeof...(Ts) / ColsV_>>;

template <size_t ColsV_, typename Tuple, typename IndexSequence>
struct ToColsImpl;
template <size_t ColsV_, typename... Ts, size_t... ColIs>
struct ToColsImpl<ColsV_, std::tuple<Ts...>, std::index_sequence<ColIs...>> {
  using type = typename Cat<typename std::tuple<typename GetSubset<ColIs, sizeof...(Ts), ColsV_, std::tuple<Ts...>>::type>...>::type;
};

template <size_t ColsV_, typename... Ts>
using ToCols = ToColsImpl<ColsV_, std::tuple<Ts...>, std::make_index_sequence<ColsV_>>;

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
  using type = std::tuple<typename Product<typename std::tuple_element<Is, std::tuple<TsLhs...>>::type,
                                           typename std::tuple_element<Is, std::tuple<TsRhs...>>::type>::type...>;
};

template <typename Lhs, typename Rhs>
struct MultiplyTwoTuples;
template <typename... TsLhs, typename... TsRhs>
struct MultiplyTwoTuples<std::tuple<TsLhs...>, std::tuple<TsRhs...>> {
  static_assert(sizeof...(TsLhs) == sizeof...(TsRhs), "Tuple sizes do not match.");
  using type = typename MultiplyTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::make_index_sequence<sizeof...(TsLhs)>>::type;
};

template <typename Lhs, typename Rhs, typename IndexSequence>
struct ContractTwoTuplesImpl;
template <typename... TsLhs, typename... TsRhs, size_t... Is>
struct ContractTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::index_sequence<Is...>> {
  using type = typename Sum<typename Product<typename std::tuple_element<Is, std::tuple<TsLhs...>>::type,
                                             typename std::tuple_element<Is, std::tuple<TsRhs...>>::type>::type...>::type;
};

template <typename Lhs, typename Rhs>
struct ContractTwoTuples;
template <typename... TsLhs, typename... TsRhs>
struct ContractTwoTuples<std::tuple<TsLhs...>, std::tuple<TsRhs...>> {
  using type = typename ContractTwoTuplesImpl<std::tuple<TsLhs...>, std::tuple<TsRhs...>, std::make_index_sequence<sizeof...(TsLhs)>>::type;
};

template <typename Lhs, typename Rhs, typename IndexSequence>
struct ContractTuples;
template <typename... TuplesLhs, typename... TuplesRhs, size_t... Is>
struct ContractTuples<std::tuple<TuplesLhs...>, std::tuple<TuplesRhs...>, std::index_sequence<Is...>> {
  using type = std::tuple<
      typename ContractTwoTuples<typename std::tuple_element<Is / sizeof...(TuplesRhs), std::tuple<TuplesLhs...>>::type,
                                 typename std::tuple_element<Is % sizeof...(TuplesRhs), std::tuple<TuplesRhs...>>::type>::type...>;
};

//! A matrix of the types in the std::tuple Types, row by row, of the size MatrixSizeT_, a MatrixSize like that of ValuesWrapper.
//! It is created with the aliases TypeMatrix, TypeVector and TypeScalar.
template <typename MatrixSizeT_, typename Types>
struct TypeMatrixImpl;
template <int RowsV_, int ColsV_, typename... Types>
struct TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, std::tuple<Types...>> {
  static_assert(sizeof...(Types) == RowsV_ * ColsV_, "Number of types does not match matrix size.");
};

template <int RowsV_, int ColsV_, typename Type0, typename... Types>
struct CreateTypeMatrix {
  static_assert(sizeof...(Types) + 1 == RowsV_ * ColsV_, "Number of types does not match matrix size.");
  using type = TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, std::tuple<Type0, Types...>>;
};
template <int RowsV_, int ColsV_, typename TypeT_>
struct CreateTypeMatrix<RowsV_, ColsV_, TypeT_> {
  using type = TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, HomogeneousTuple<RowsV_ * ColsV_, TypeT_>>;
};

template <int RowsV_, int ColsV_, typename Types>
struct IsHomogeneous<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, Types>> {
  static constexpr bool value = IsHomogeneous<Types>::value;
};

template <int RowsV_, int ColsV_, typename Types>
struct GetTypes<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, Types>> {
  using type = Types;
};

template <int RowsV_, int ColsV_, typename Types>
struct GetType<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, Types>> {
  static_assert(IsHomogeneous<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, Types>>::value, "Types are heterogeneous.");
  using type = typename std::tuple_element<0, typename GetTypes<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, Types>>::type>::type;
};

template <typename Types>
struct Dimension<TypeMatrixImpl<MatrixSize<1, 1>, Types>> {
  static constexpr int value = 0;
};
template <int RowsV_, typename Types>
struct Dimension<TypeMatrixImpl<MatrixSize<RowsV_, 1>, Types>> {
  static constexpr int value = 1;
};
template <int ColsV_, typename Types>
struct Dimension<TypeMatrixImpl<MatrixSize<1, ColsV_>, Types>> {
  static constexpr int value = 1;
};
template <int RowsV_, int ColsV_, typename Types>
struct Dimension<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, Types>> {
  static constexpr int value = 2;
};

template <int RowsV_, int ColsV_, typename Types>
struct Rows<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, Types>> {
  static constexpr int value = RowsV_;
};

template <int RowsV_, int ColsV_, typename Types>
struct Cols<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, Types>> {
  static constexpr int value = ColsV_;
};

template <int RowsV_, int ColsV_, typename... Types, int ElementRowIdV_, int ElementColIdV_>
struct Element<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, std::tuple<Types...>>, ElementRowIdV_, ElementColIdV_> {
  static_assert(internal::RequireElementInRange<MatrixSize<RowsV_, ColsV_>, ElementRowIdV_, ElementColIdV_>::value);
  using type = typename std::tuple_element<std::size_t{ElementRowIdV_ * ColsV_ + ElementColIdV_}, std::tuple<Types...>>::type;
};

template <int ColsV_, int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_, typename Tuple, typename IndexSequence>
struct BlockImpl;
template <int ColsV_, int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_, typename... Ts, size_t... BlockRowIs>
struct BlockImpl<ColsV_, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_, std::tuple<Ts...>, std::index_sequence<BlockRowIs...>> {
  using type = typename Cat<
      typename GetSubset<(BlockRowIdV_ + BlockRowIs) * ColsV_ + BlockColIdV_,
                         (BlockRowIdV_ + BlockRowIs) * ColsV_ + BlockColIdV_ + BlockColsV_, 1, std::tuple<Ts...>>::type...>::type;
};

template <int RowsV_, int ColsV_, typename... Types, int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
struct Block<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, std::tuple<Types...>>, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_> {
  static_assert(BlockRowIdV_ >= 0 && BlockColIdV_ >= 0, "Block top left corner is out of range.");
  static_assert(BlockRowIdV_ + BlockRowsV_ <= RowsV_ && BlockColIdV_ + BlockColsV_ <= ColsV_, "Block bottom right corner is out of range.");
  using type = TypeMatrixImpl<MatrixSize<BlockRowsV_, BlockColsV_>,
                              typename BlockImpl<ColsV_, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_, std::tuple<Types...>,
                                                 std::make_index_sequence<std::size_t{BlockRowsV_}>>::type>;
};

template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, int RowsRhsV_, int ColsRhsV_, typename... TypesRhs>
struct Sum<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>,
           TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhsV_ == RowsRhsV_, "Matrix sizes do not match.");
  static_assert(ColsLhsV_ == ColsRhsV_, "Matrix sizes do not match.");
  using type =
      TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsRhsV_>, typename AddTwoTuples<std::tuple<TypesLhs...>, std::tuple<TypesRhs...>>::type>;
};

template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, int RowsRhsV_, int ColsRhsV_, typename... TypesRhs>
struct Difference<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>,
                  TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhsV_ == RowsRhsV_, "Matrix sizes do not match.");
  static_assert(ColsLhsV_ == ColsRhsV_, "Matrix sizes do not match.");
  using type =
      TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsRhsV_>, typename SubtractTwoTuples<std::tuple<TypesLhs...>, std::tuple<TypesRhs...>>::type>;
};

// Product of type matrix and type.
template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, typename TypeRhs>
struct Product<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>, TypeRhs> {
  using type = TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<typename Product<TypesLhs, TypeRhs>::type...>>;
};
// Product of type and type matrix.
template <typename TypeLhs, int RowsRhsV_, int ColsRhsV_, typename... TypesRhs>
struct Product<TypeLhs, TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<TypesRhs...>>> {
  using type = typename Product<TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<TypesRhs...>>, TypeLhs>::type;
};
// Product of two type matrices. The right one is a 1x1 matrix, the left one can be a 1x1 matrix as well.
template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, typename TypeRhs>
struct Product<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>,
               TypeMatrixImpl<MatrixSize<1, 1>, std::tuple<TypeRhs>>> {
  using type = typename Product<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>, TypeRhs>::type;
};
// Product of two type matrices. The left one is a 1x1 matrix, the right one is not.
template <typename TypeLhs, int RowsRhsV_, int ColsRhsV_, typename Type0Rhs, typename Type1Rhs, typename... TypesRhs>
struct Product<TypeMatrixImpl<MatrixSize<1, 1>, std::tuple<TypeLhs>>,
               TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<Type0Rhs, Type1Rhs, TypesRhs...>>> {
  using type =
      typename Product<TypeLhs, TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<Type0Rhs, Type1Rhs, TypesRhs...>>>::type;
};
// Product of two type matrices. None of them is a 1x1 matrix.
template <int RowsLhsV_, int ColsLhsV_, typename Type0Lhs, typename Type1Lhs, typename... TypesLhs, int RowsRhsV_, int ColsRhsV_,
          typename Type0Rhs, typename Type1Rhs, typename... TypesRhs>
struct Product<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<Type0Lhs, Type1Lhs, TypesLhs...>>,
               TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<Type0Rhs, Type1Rhs, TypesRhs...>>> {
  static_assert(RowsLhsV_ == RowsRhsV_, "Matrix sizes do not match.");
  static_assert(ColsLhsV_ == ColsRhsV_, "Matrix sizes do not match.");
  using type =
      TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, typename MultiplyTwoTuples<std::tuple<Type0Lhs, Type1Lhs, TypesLhs...>,
                                                                                  std::tuple<Type0Rhs, Type1Rhs, TypesRhs...>>::type>;
};

template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, int RowsRhsV_, int ColsRhsV_, typename... TypesRhs>
struct InnerProduct<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>,
                    TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<TypesRhs...>>> {
  static_assert(ColsLhsV_ == RowsRhsV_, "Matrix sizes do not match.");
  using type = TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsRhsV_>,
                              typename ContractTuples<typename ToRows<std::size_t{ColsLhsV_}, TypesLhs...>::type,
                                                      typename ToCols<std::size_t{ColsRhsV_}, TypesRhs...>::type,
                                                      std::make_index_sequence<std::size_t{RowsLhsV_ * ColsRhsV_}>>::type>;
};

template <int N, typename T>
struct CreateHomogeneousTupleImpl {
  template <typename... Args>
  using type = typename CreateHomogeneousTupleImpl<N - 1, T>::template type<T, Args...>;
};

template <typename T>
struct CreateHomogeneousTupleImpl<0, T> {
  template <typename... Args>
  using type = std::tuple<Args...>;
};

template <int N, typename T>
struct CreateHomogeneousTuple {
  using type = typename CreateHomogeneousTupleImpl<N, T>::template type<>;
};

template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, int RowsRhsV_, int ColsRhsV_, typename... TypesRhs>
struct OuterProduct<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>,
                    TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<TypesRhs...>>> {
  static_assert(std::min(RowsLhsV_, RowsRhsV_) <= 1,
                "Hypermatrices are not supported. At least one of the matrices must be a scalar or a vector.");
  static_assert(std::min(ColsLhsV_, ColsRhsV_) <= 1,
                "Hypermatrices are not supported. At least one of the matrices must be a scalar or a vector.");
  using type = TypeMatrixImpl<
      MatrixSize<RowsLhsV_ + RowsRhsV_ - 1, ColsLhsV_ + ColsRhsV_ - 1>,
      typename IfThenElse<
          RowsLhsV_ == 1,
          typename Cat<typename MultiplyTwoTuples<std::tuple<TypesLhs...>,
                                                  typename CreateHomogeneousTuple<sizeof...(TypesLhs), TypesRhs>::type>::type...>::type,
          typename Cat<typename MultiplyTwoTuples<
              std::tuple<TypesRhs...>, typename CreateHomogeneousTuple<sizeof...(TypesRhs), TypesLhs>::type>::type...>::type>::type>;
};

template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, int RowsRhsV_, int ColsRhsV_, typename... TypesRhs>
struct DotProduct<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>,
                  TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhsV_ == RowsRhsV_, "Matrix sizes do not match.");
  static_assert(ColsLhsV_ == 1, "Dot product is only defined for vectors.");
  static_assert(ColsRhsV_ == 1, "Dot product is only defined for vectors.");
  using type = typename InnerProduct<typename Transpose<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>>::type,
                                     TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<TypesRhs...>>>::type;
};

template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, int RowsRhsV_, int ColsRhsV_, typename... TypesRhs>
struct CrossProduct<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>,
                    TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhsV_ == 3 && RowsRhsV_ == 3 && ColsLhsV_ == 1 && ColsRhsV_ == 1,
                "Cross product is only defined for three-dimensional vectors.");
  using type =
      TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>,
                     std::tuple<typename Difference<typename Product<std::tuple_element_t<1, std::tuple<TypesLhs...>>,
                                                                     std::tuple_element_t<2, std::tuple<TypesRhs...>>>::type,
                                                    typename Product<std::tuple_element_t<2, std::tuple<TypesLhs...>>,
                                                                     std::tuple_element_t<1, std::tuple<TypesRhs...>>>::type>::type,
                                typename Difference<typename Product<std::tuple_element_t<2, std::tuple<TypesLhs...>>,
                                                                     std::tuple_element_t<0, std::tuple<TypesRhs...>>>::type,
                                                    typename Product<std::tuple_element_t<0, std::tuple<TypesLhs...>>,
                                                                     std::tuple_element_t<2, std::tuple<TypesRhs...>>>::type>::type,
                                typename Difference<typename Product<std::tuple_element_t<0, std::tuple<TypesLhs...>>,
                                                                     std::tuple_element_t<1, std::tuple<TypesRhs...>>>::type,
                                                    typename Product<std::tuple_element_t<1, std::tuple<TypesLhs...>>,
                                                                     std::tuple_element_t<0, std::tuple<TypesRhs...>>>::type>::type>>;
};

// Division of a type matrix by a type.
template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, typename TypeRhs>
struct Division<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>, TypeRhs> {
  using type = TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<typename Division<TypesLhs, TypeRhs>::type...>>;
};
// Division of a type matrix by a 1x1 type matrix.
template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, typename TypeRhs>
struct Division<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>,
                TypeMatrixImpl<MatrixSize<1, 1>, std::tuple<TypeRhs>>> {
  using type = typename Division<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>, TypeRhs>::type;
};
// Division of a type matrix by a type matrix.
template <int RowsLhsV_, int ColsLhsV_, typename... TypesLhs, int RowsRhsV_, int ColsRhsV_, typename... TypesRhs>
struct Division<TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<TypesLhs...>>,
                TypeMatrixImpl<MatrixSize<RowsRhsV_, ColsRhsV_>, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhsV_ == RowsRhsV_, "Matrix sizes do not match.");
  static_assert(ColsLhsV_ == ColsRhsV_, "Matrix sizes do not match.");
  using type = TypeMatrixImpl<MatrixSize<RowsLhsV_, ColsLhsV_>, std::tuple<typename Division<TypesLhs, TypesRhs>::type...>>;
};

//! The elementwise reciprocal. The matrix inverse is only defined for matrices of units (see Units.hpp).
template <int RowsV_, int ColsV_, typename... Types>
struct CwiseInverse<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, std::tuple<Types...>>> {
  using type = TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, std::tuple<typename Inverse<Types>::type...>>;
};

//! The elementwise power, like the product of type matrices, which is elementwise too, e.g. the units of pow<2>() of quantities.
template <int RowsV_, int ColsV_, typename... Types, typename ExponentT_>
struct Power<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, std::tuple<Types...>>, ExponentT_> {
  using type = TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, std::tuple<typename Power<Types, ExponentT_>::type...>>;
};

template <int RowsV_, int ColsV_, typename... Types>
struct Transpose<TypeMatrixImpl<MatrixSize<RowsV_, ColsV_>, std::tuple<Types...>>> {
  using type =
      TypeMatrixImpl<MatrixSize<ColsV_, RowsV_>, typename ExtractAndApply<Cat, typename ToCols<std::size_t{ColsV_}, Types...>::type>::type>;
};

template <int SizeV_, typename ZeroType, typename OneType, typename IndexSequence>
struct DiagonalTypeMatrixImpl;
template <int SizeV_, typename ZeroType, typename OneType, size_t... Is>
struct DiagonalTypeMatrixImpl<SizeV_, ZeroType, OneType, std::index_sequence<Is...>> {
  using type =
      TypeMatrixImpl<MatrixSize<SizeV_, SizeV_>, std::tuple<typename IfThenElse<Is % (SizeV_ + 1) != 0, ZeroType, OneType>::type...>>;
};

template <int SizeV_, typename ZeroType, typename OneType>
using DiagonalTypeMatrix =
    typename DiagonalTypeMatrixImpl<SizeV_, ZeroType, OneType, std::make_index_sequence<std::size_t{SizeV_ * SizeV_}>>::type;

/**
 * A matrix storing types at compile-time.
 * @tparam RowsV_ Number of rows.
 * @tparam ColsV_ Number of columns.
 * @tparam Types Types stored in the matrix, row-major order. The number of types must match the number of elements the matrix can store.
 */
template <int RowsV_, int ColsV_, typename... Types>
using TypeMatrix = typename CreateTypeMatrix<RowsV_, ColsV_, Types...>::type;

/**
 * A vector storing types at compile-time.
 * @tparam RowsV_ Number of rows.
 * @tparam Types Types stored in the vector. The number of types must match the number of elements the vector can store.
 */
template <int RowsV_, typename... Types>
using TypeVector = typename CreateTypeMatrix<RowsV_, 1, Types...>::type;

/**
 * A scalar storing a type at compile-time.
 * @tparam TypeT_ Type stored in the scalar.
 */
template <typename TypeT_>
using TypeScalar = typename CreateTypeMatrix<1, 1, TypeT_>::type;

}  // namespace wuzp
