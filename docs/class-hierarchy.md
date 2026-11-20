# Class hierarchy

## Values, quantities and tensors

A tensor holds quantities, which hold the values of a value matrix backend and a matrix of units:

```mermaid
classDiagram
  direction LR
  class ValuesWrapper {
    the values of a backend
  }
  class Quantities {
    values with a unit per element
  }
  class TypeMatrix {
    the units, one per element
  }
  class Tensor {
    quantities in a coordinate system
  }
  class CoordinateSystem {
    identifier and metric
  }
  class Indices {
    the variance of each index
  }
  Tensor *-- Quantities
  Tensor ..> CoordinateSystem
  Tensor ..> Indices
  Quantities *-- ValuesWrapper
  Quantities ..> TypeMatrix
```

* `wuzp_eigen::ValueMatrix<Value, Rows, Cols>` and `wuzp_blitz::ValueMatrix` are `ValuesWrapper`s of the arrays of their libraries.
* `Quantity<Values, Unit>` is `Quantities` of a 1x1 matrix with one unit, and `wuzp_eigen::QuantityD<Unit>` a quantity with a `double` value.
* `Vector`, `Covector`, `Point` and the other tensor aliases, e.g. `PositionPoint3` or `ForceCovector3`, are `Tensor`s with fixed indices and absoluteness.
* The unit types and the coordinate systems are tags: they exist only at compile time.

## Transformations

Every transformation derives from `ActiveTransformation` or `PassiveTransformation`, which provide `transform()`, composition with `*` and `inverse()` through the [CRTP](https://en.cppreference.com/w/cpp/language/crtp), and these from `Transformation<DerivedT_>`, whose destructor is protected and not virtual: transformations are values, never deleted through a pointer to the base.
The base is a template of the transformation, so that a proper rigid transformation and its rotation have different empty bases, which take no space: a proper rigid transformation is as large as its rotation and its translation.
`AnyTransformation` tells whether a type is a transformation.

```mermaid
classDiagram
  class Transformation
  class ActiveTransformation
  class PassiveTransformation
  Transformation <|-- ActiveTransformation
  Transformation <|-- PassiveTransformation
  ActiveTransformation <|-- ActiveLinearTransformation
  ActiveTransformation <|-- ActiveSpatialRotation
  ActiveTransformation <|-- ActiveTranslation
  ActiveTransformation <|-- ActiveProperRigidTransformationTemplate
  PassiveTransformation <|-- PassiveLinearTransformation
  PassiveTransformation <|-- PassiveSpatialRotation
  PassiveTransformation <|-- PassiveTranslation
  PassiveTransformation <|-- PassiveProperRigidTransformationTemplate
  PassiveTransformation <|-- LorentzTransformation
  PassiveTransformation <|-- Jacobian
  ActiveProperRigidTransformationTemplate *-- ActiveSpatialRotation : rotation
  ActiveProperRigidTransformationTemplate *-- ActiveTranslation : translation
  PassiveProperRigidTransformationTemplate *-- PassiveSpatialRotation : rotation
  PassiveProperRigidTransformationTemplate *-- PassiveTranslation : translation
```

* `ActiveProperRigidTransformation3` and `PassiveProperRigidTransformation3` are the templates of proper rigid transformations with a spatial rotation and a spatial translation, `ActiveSpatialTranslation3` and `PassiveSpatialTranslation3` translations in units of length.
* The rotations and the Lorentz transformations transform the same tensors as the linear transformations with their matrices, whose traits they inherit, but have structural inverses and check their validity in debug builds.
* Which tensors a transformation transforms, and which compositions exist, is decided by the traits `TransformationTraits` and `CompositionTraits`, not by virtual functions (see [Transformations](concepts/transformations.md)).

## Spatial vectors

The spatial vectors derive from `SpatialVector<DerivedT_, RotationalT_, TranslationalT_>` through the CRTP, which holds their two parts, tensors expressed in the same coordinate system, and provides their getters, comparisons and sums.
Its destructor is protected and not virtual, like that of `Transformation`: spatial vectors are values, never deleted through a pointer to the base.
`AnySpatialVector` tells whether a type is a spatial vector.

```mermaid
classDiagram
  class SpatialVector {
    rotational and translational part
  }
  class Tensor {
    quantities in a coordinate system
  }
  SpatialVector <|-- SpatialVelocity
  SpatialVector <|-- SpatialAcceleration
  SpatialVector <|-- SpatialForce
  SpatialVector *-- Tensor : rotational
  SpatialVector *-- Tensor : translational
```

* `SpatialVelocity` holds an `AngularVelocityVector3` and a `VelocityVector3`, `SpatialAcceleration` an `AngularAccelerationVector3` and an `AccelerationVector3`, and `SpatialForce` a `TorqueVector3` and a `ForceVector3`.
* Their frames and their point are coordinate systems, tags like the coordinate system they are expressed in (see [Spatial vectors](concepts/spatial-vectors.md)).
* They are not transformations, and the transformations do not derive from them: passive rotations and translations, and thus passive proper rigid transformations, re-express them through `TransformationTraits`, and `adjoint()` maps them by a `PassiveProperRigidTransformationTemplate`.

## Backends

The core reaches the libraries only through traits, e.g. `Sum`, `Transpose` or `Coefficient`, which the backends specialize for `AccessImplementation<Implementation>`, and through `AccessUnits` for the units.
The concepts `ValueMatrixBackend`, `InvertibleValueMatrixBackend` and `UnitSystem` check that a backend provides them (see [Backends](backends.md)).
