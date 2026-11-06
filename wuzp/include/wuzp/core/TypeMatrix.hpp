#pragma once

#include <cstddef>
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

// TODO(remod): Use MatrixSize here.
template <int Rows, int Cols, typename Types>
struct TypeMatrixImpl;
template <int Rows, int Cols, typename... Types>
struct TypeMatrixImpl<Rows, Cols, std::tuple<Types...>> {
  static_assert(sizeof...(Types) == Rows * Cols, "Number of types does not match matrix size.");
};

template <int Rows, int Cols, typename Type0, typename... Types>
struct CreateTypeMatrix {
  static_assert(sizeof...(Types) + 1 == Rows * Cols, "Number of types does not match matrix size.");
  using type = TypeMatrixImpl<Rows, Cols, std::tuple<Type0, Types...>>;
};
template <int Rows, int Cols, typename Type>
struct CreateTypeMatrix<Rows, Cols, Type> {
  using type = TypeMatrixImpl<Rows, Cols, HomogeneousTuple<Rows * Cols, Type>>;
};

template <int Rows, int Cols, typename Types>
struct IsHomogeneous<TypeMatrixImpl<Rows, Cols, Types>> {
  static constexpr bool value = IsHomogeneous<Types>::value;
};

template <int Rows, int Cols, typename Types>
struct GetTypes<TypeMatrixImpl<Rows, Cols, Types>> {
  using type = Types;
};

template <int Rows, int Cols, typename Types>
struct GetType<TypeMatrixImpl<Rows, Cols, Types>> {
  static_assert(IsHomogeneous<TypeMatrixImpl<Rows, Cols, Types>>::value, "Types are heterogeneous.");
  using type = typename std::tuple_element<0, typename GetTypes<TypeMatrixImpl<Rows, Cols, Types>>::type>::type;
};

template <typename Types>
struct Dimension<TypeMatrixImpl<1, 1, Types>> {
  static constexpr int value = 0;
};
template <int RowsV_, typename Types>
struct Dimension<TypeMatrixImpl<RowsV_, 1, Types>> {
  static constexpr int value = 1;
};
template <int ColsV_, typename Types>
struct Dimension<TypeMatrixImpl<1, ColsV_, Types>> {
  static constexpr int value = 1;
};
template <int RowsV_, int ColsV_, typename Types>
struct Dimension<TypeMatrixImpl<RowsV_, ColsV_, Types>> {
  static constexpr int value = 2;
};

template <int RowsV_, int ColsV_, typename Types>
struct Rows<TypeMatrixImpl<RowsV_, ColsV_, Types>> {
  static constexpr int value = RowsV_;
};

template <int RowsV_, int ColsV_, typename Types>
struct Cols<TypeMatrixImpl<RowsV_, ColsV_, Types>> {
  static constexpr int value = ColsV_;
};

template <int Rows, int Cols, typename... Types, int ElementRowId, int ElementColId>
struct Element<TypeMatrixImpl<Rows, Cols, std::tuple<Types...>>, ElementRowId, ElementColId> {
  static_assert(ElementRowId >= 0 && ElementRowId < Rows, "Element row id is out of range.");
  static_assert(ElementColId >= 0 && ElementColId < Cols, "Element columns id is out of range.");
  using type = typename std::tuple_element<static_cast<std::size_t>(ElementRowId* Cols + ElementColId), std::tuple<Types...>>::type;
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
                                                 std::make_index_sequence<static_cast<std::size_t>(BlockRows)>>::type>;
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

// Product of type matrix and type.
template <int RowsLhs, int ColsLhs, typename... TypesLhs, typename TypeRhs>
struct Product<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeRhs> {
  using type = TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<typename Product<TypesLhs, TypeRhs>::type...>>;
};
// Product of type and type matrix.
template <typename TypeLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct Product<TypeLhs, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  using type = typename Product<TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>, TypeLhs>::type;
};
// Product of two type matrices. The right one is a 1x1 matrix, the left one can be a 1x1 matrix as well.
template <int RowsLhs, int ColsLhs, typename... TypesLhs, typename TypeRhs>
struct Product<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<1, 1, std::tuple<TypeRhs>>> {
  using type = typename Product<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeRhs>::type;
};
// Product of two type matrices. The left one is a 1x1 matrix, the right one is not.
template <typename TypeLhs, int RowsRhs, int ColsRhs, typename Type0Rhs, typename Type1Rhs, typename... TypesRhs>
struct Product<TypeMatrixImpl<1, 1, std::tuple<TypeLhs>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<Type0Rhs, Type1Rhs, TypesRhs...>>> {
  using type = typename Product<TypeLhs, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<Type0Rhs, Type1Rhs, TypesRhs...>>>::type;
};
// Product of two type matrices. None of them is a 1x1 matrix.
template <int RowsLhs, int ColsLhs, typename Type0Lhs, typename Type1Lhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename Type0Rhs,
          typename Type1Rhs, typename... TypesRhs>
struct Product<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<Type0Lhs, Type1Lhs, TypesLhs...>>,
               TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<Type0Rhs, Type1Rhs, TypesRhs...>>> {
  static_assert(RowsLhs == RowsRhs, "Matrix sizes do not match.");
  static_assert(ColsLhs == ColsRhs, "Matrix sizes do not match.");
  using type = TypeMatrixImpl<
      RowsLhs, ColsLhs,
      typename MultiplyTwoTuples<std::tuple<Type0Lhs, Type1Lhs, TypesLhs...>, std::tuple<Type0Rhs, Type1Rhs, TypesRhs...>>::type>;
};

template <int RowsLhs, int ColsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct InnerProduct<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  static_assert(ColsLhs == RowsRhs, "Matrix sizes do not match.");
  using type = TypeMatrixImpl<RowsLhs, ColsRhs,
                              typename ContractTuples<typename ToRows<static_cast<std::size_t>(ColsLhs), TypesLhs...>::type,
                                                      typename ToCols<static_cast<std::size_t>(ColsRhs), TypesRhs...>::type,
                                                      std::make_index_sequence<static_cast<std::size_t>(RowsLhs* ColsRhs)>>::type>;
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

template <int RowsLhs, int ColsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct OuterProduct<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  static_assert(std::min(RowsLhs, RowsRhs) <= 1,
                "Hypermatrices are not supported. At least one of the matrices must be a scalar or a vector.");
  static_assert(std::min(ColsLhs, ColsRhs) <= 1,
                "Hypermatrices are not supported. At least one of the matrices must be a scalar or a vector.");
  using type = TypeMatrixImpl<
      RowsLhs + RowsRhs - 1, ColsLhs + ColsRhs - 1,
      typename IfThenElse<
          RowsLhs == 1,
          typename Cat<typename MultiplyTwoTuples<std::tuple<TypesLhs...>,
                                                  typename CreateHomogeneousTuple<sizeof...(TypesLhs), TypesRhs>::type>::type...>::type,
          typename Cat<typename MultiplyTwoTuples<
              std::tuple<TypesRhs...>, typename CreateHomogeneousTuple<sizeof...(TypesRhs), TypesLhs>::type>::type...>::type>::type>;
};

template <int RowsLhs, int ColsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct DotProduct<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhs == RowsRhs, "Matrix sizes do not match.");
  static_assert(ColsLhs == 1, "Dot product is only defined for vectors.");
  static_assert(ColsRhs == 1, "Dot product is only defined for vectors.");
  using type = typename InnerProduct<typename Transpose<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>>::type,
                                     TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>>::type;
};

template <int RowsLhs, int ColsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct CrossProduct<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhs == 3 && RowsRhs == 3 && ColsLhs == 1 && ColsRhs == 1,
                "Cross product is only defined for three-dimensional vectors.");
  using type =
      TypeMatrixImpl<RowsLhs, ColsLhs,
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
template <int RowsLhs, int ColsLhs, typename... TypesLhs, typename TypeRhs>
struct Division<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeRhs> {
  using type = TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<typename Division<TypesLhs, TypeRhs>::type...>>;
};
// Division of a type matrix by a 1x1 type matrix.
template <int RowsLhs, int ColsLhs, typename... TypesLhs, typename TypeRhs>
struct Division<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<1, 1, std::tuple<TypeRhs>>> {
  using type = typename Division<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeRhs>::type;
};
// Division of a type matrix by a type matrix.
template <int RowsLhs, int ColsLhs, typename... TypesLhs, int RowsRhs, int ColsRhs, typename... TypesRhs>
struct Division<TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<TypesLhs...>>, TypeMatrixImpl<RowsRhs, ColsRhs, std::tuple<TypesRhs...>>> {
  static_assert(RowsLhs == RowsRhs, "Matrix sizes do not match.");
  static_assert(ColsLhs == ColsRhs, "Matrix sizes do not match.");
  using type = TypeMatrixImpl<RowsLhs, ColsLhs, std::tuple<typename Division<TypesLhs, TypesRhs>::type...>>;
};

template <int Rows, int Cols, typename... Types>
struct Inverse<TypeMatrixImpl<Rows, Cols, std::tuple<Types...>>> {
  using type = TypeMatrixImpl<Rows, Cols, std::tuple<typename Inverse<Types>::type...>>;
};

template <int Rows, int Cols, typename... Types>
struct Transpose<TypeMatrixImpl<Rows, Cols, std::tuple<Types...>>> {
  using type =
      TypeMatrixImpl<Cols, Rows, typename ExtractAndApply<Cat, typename ToCols<static_cast<std::size_t>(Cols), Types...>::type>::type>;
};

template <int Size, typename ZeroType, typename OneType, typename IndexSequence>
struct DiagonalTypeMatrixImpl;
template <int Size, typename ZeroType, typename OneType, size_t... Is>
struct DiagonalTypeMatrixImpl<Size, ZeroType, OneType, std::index_sequence<Is...>> {
  using type = TypeMatrixImpl<Size, Size, std::tuple<typename IfThenElse<Is % (Size + 1) != 0, ZeroType, OneType>::type...>>;
};

template <int Size, typename ZeroType, typename OneType>
using DiagonalTypeMatrix =
    typename DiagonalTypeMatrixImpl<Size, ZeroType, OneType, std::make_index_sequence<static_cast<std::size_t>(Size* Size)>>::type;

/**
 * A matrix storing types at compile-time.
 * @tparam Rows Number of rows.
 * @tparam Cols Number of columns.
 * @tparam Types Types stored in the matrix, row-major order. The number of types must match the number of elements the matrix can store.
 */
template <int Rows, int Cols, typename... Types>
using TypeMatrix = typename CreateTypeMatrix<Rows, Cols, Types...>::type;

/**
 * A vector storing types at compile-time.
 * @tparam Rows Number of rows.
 * @tparam Types Types stored in the vector. The number of types must match the number of elements the vector can store.
 */
template <int Rows, typename... Types>
using TypeVector = typename CreateTypeMatrix<Rows, 1, Types...>::type;

/**
 * A scalar storing a type at compile-time.
 * @tparam Type Type stored in the scalar.
 */
template <typename Type>
using TypeScalar = typename CreateTypeMatrix<1, 1, Type>::type;

}  // namespace wuzp
