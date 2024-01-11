# Design Considerations

### Handling of Coordinate System Transformations

Options:

1. Handle coordinate system transformations on the level where the transformation objects operate.
   The coordinate system is assigned to the transformed object.
1. Handle coordinate system transformations deeper using a matrix of coordinate system transformation objects.
   A coordinate system transformation object has a "from" and a "to" coordinate system.
   Whenever a coordinate system transformation object is applied to an object with a "from" coordinate system, it turns into a "to" coordinate system.

Decision: 1.

Transforming certain tensors, e.g. a mass matrix, requires left and right multiplication with a transformation object.
This rules out option 2, as the two multiplications do not know about each other.
The information that the new coordinate system needs to be applied is only available on the level of the transformation object.

### Tensor Calculus

* [Tensor Operations: Contractions, Inner Products, Outer Products (Video)](https://www.youtube.com/watch?v=fDcfqfWRIi8&ab_channel=FacultyofKhan)
* [Matrix multiplication and tensorial summation convention](https://physics.stackexchange.com/questions/411015/matrix-multiplication-and-tensorial-summation-convention)
* [Converting between matrix multiplication and tensor contraction](https://physics.stackexchange.com/questions/252473/converting-between-matrix-multiplication-and-tensor-contraction)


// Tensor (3+) - Hypermatrix
// Tensor (2)  - Matrix
// Tensor (1)  - Vector
// Tensor (0)  - Scalar

// template <int ContravariantIndices, int CovariantIndices>
// class Tensor;
//
// using Scalar = Tensor<0, 0>;
// using EuclideanVector = Tensor<1, 0>;       // A * v
// using PassiveLinearTransformation = Tensor<1, 1>;  // M' = A^-1 * M * A
// using RotationMatrix = Tensor<1, 1>;        // M' = A^-1 * M * A
// using InertiaTensor = Tensor<0, 2>;         // M' = A^T * M * A

// n-m-Tensor:
// n = contravariant
// m = covariant

// 1-1-Tensor aka linear transformation in our convention:
// w = M * v
// w' = M' * v'
// w' = A * w = A * M * v = A * M * A^-1 * A * v = A * M * A^-1 * v'
// M' = A * M * A^-1

// 0-2-Tensor aka inertia tensor (aka mass matrix) in our convention:
// E = v^T * M * v
// E' = v^T' * M' * v'
// E' = E
// v^T * M * v = v^T * A^T * M' * A * v
// M = A^T * M' * A
// M' = (A^-1)^T * M * A^-1
// Our convention is confusing as we have two times A^-1, which intuitively corresponds to 2 contravariant indices.
// Probably this is due basis vectors being transformed actively while the measured quantities in these coordinate systems are transformed
// passively.

// 2-0-Tensor in our convention:
// Should be invariant?

// Einstein Summation

// A = A^i_j
// A^T = A_j^i

// https://en.wikipedia.org/wiki/Change_of_basis

// Note: Wiki uses the different convention "old = A * new", we use "new = A * old".

// e^i = A^i_j * e^j' = A * e'
// e^i' = (A^-1)^i_j * e^j = A^-1 * e
// e_i' = e_j * (A^-1)_j^i = e^T * (A^-1)^T

// https://en.wikipedia.org/wiki/Tensor

// Basis vector (row vector!!!)
// e_i' = e_j * A_i^j
// e' = e * A

// In our convention (column vector!!!), redefine f = e^T, f' = e^T', B = A^T:
// f^T' = f^T * B^T
// f' = B * f

// Contravariant vector (or simply "vector"):
// v^i, column vector
// v^i' = (A^-1)^i_j * v^j
// v' = A^-1 * v
// In our convention: v' = A * v

// Covariant:
// w_i, row vector
// w_i' = w_j * A_i^j
// w' = w * A
// In our convention: w' = w * A^-1

// Orthogonal matrix / transformation: A^-1 == A^T