# Data Structures

The keyword `data` is used to define a new data type. Data structures in Tamgu are mapped over frames.

- `Eq`: This data structure inserts code to compare objects. It defines a behavior that returns true if the object's fields are the same, or false otherwise.
- `Ord`: This data structure inserts code to compare objects with operators such as `<`, `>`, `<=`, and `>=`, similar to `Eq`.

Example:

We can extend these descriptions in a more refined way. For instance, we could decide to replace the coordinates with a `Point` data structure.

First, we declare a `Point` data structure:
```haskell
data Point = Point Float Float deriving (Show)
```

Which is then used to replace coordinates:
```haskell
data Shape = Circle Point Float | Rect Point Point deriving (Show)
```

We modify our `Point` with the `b` offset:
```haskell
dep :: Point -> Float -> Point
dep (Point x y) b = Point (x + b) (y + b)
```

Our movement function will modify a `Shape` object. It takes a `Shape` object and a `Float` and returns a `Shape` object. Note the use of the `$` operator, which replaces `<..>` around the last expression:
```haskell
mouve :: Shape -> Float -> Shape
mouve (Circle p z) b = Circle (dep p b) z
mouve (Rect p1 p2) b = Rect (dep p1 b) $ dep p2 b
```

Equivalent to:
```haskell
mouve (Rect p1 p2) b = Rect (dep p1 b) (dep p2 b)
```

Let `be = mouve (Circle (Point 17 18) 30) 40`:
```haskell
let be = mouve (Circle (Point 17 18) 30) 40
println be -- Result is: Circle (Point 57 58) 30
```

Let `br = mouve (Rect (Point 17 18) (Point 30 40)) 40`:
```haskell
let br = mouve (Rect (Point 17 18) (Point 30 40)) 40
println br -- Result is: Rect (Point 57 58) (Point 70 80)
```

## Data Structure with Field Names (Monades)

You can declare a data structure with field names, which will be mapped over a frame (as mentioned above).

You describe fields in a data structure between braces (`{}`). Each declaration is a field name followed by its type:
```haskell
data Person = Person { name :: String, lastname :: String, age :: Int }
```

The system will automatically implement each field as a frame variable. It also creates as many functions (`_name()`, `_lastname()`, `_age()`) as there are fields in the frame. The function name is an alteration of the field name with a "_" as a prefix.

You can create a new data structure either with the same structure as stated in the previous paragraph, or you can directly instantiate each field separately:
```haskell
let nr = Person "Pierre" "Jean" 46 -- Same as above
let nr = Person { lastname = "Jean", name = "Dupont", age = 40 } -- Via fields
```

As you can see in the examples above, when you initialize a structure via fields, the order is no longer important.

## Enriching a Data Structure with Functions

You can also enrich a data structure with a function. For instance, if we want the above data structure to return a field, we can do:
```haskell
data Person = Name() = name
```

The frame `Person` will now expose a `Name` function that returns the field `name`. We can now call `nr.Name()`.

## Data Structures are Frames

Actually, data structures are maps over frames. For example:
```haskell
data Shape = Circle Float Float Float | Rectangle Float Float Float Float deriving (Show)
```

is actually mapped as:
```haskell
frame Shape {
    frame Circle {
        Float d1_Circle;
        Float d2_Circle;
        function _name() {
            return (name);
        }
        function _lastname() {
            return (name);
        }
        function _age() {
            return (name);
        }
    }
}
```

Note that a function has been created for each field to access its content. Also, note the presence of an `_initial()` function without any arguments, which is used to assign values to field names at creation.