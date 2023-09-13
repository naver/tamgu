# Self-included Containers

In Tamgu, there are two types of containers: value containers and object containers.

## Value Containers
Value containers include svector, ivector, and mapii. These containers hold values.

## Object Containers
Object containers include vector, map, treemap, table, and list. These containers hold objects.

## Loops in Object Containers
It is possible to include a container within another container, even including a container within itself. However, when a container includes itself, the entire structure cannot be displayed. Instead, the system will replace the self-reference with "[...]" or "{...}" if it is a map.

Example:

```cpp
vector v = [1..5];
v.push(v);
println(v);
```

Output: `v is: [1,2,3,4,5,[...]]`, where "[...]" indicates a self-reference.

It is important to note that if a container contains itself, there may be an issue with traversing the structure, as it could result in an infinite loop.

## Marking Methods
Tamgu provides three methods to help deal with this problem:

a) `mark(byte v)`: This method puts a mark on an object container.

b) `mark()`: This method returns the mark on an object container.

c) `unmark()`: This method unmarks all marks within an object container.

Example:

```cpp
vector v = [10..50:10];
v.push([60..80:10]);
v.push(v);
println(v);

function traversal(self v, int i) {
  if (i == v.size()) {
    println('End of vector');
    return;
  }
  if (v[i].mark() == 1) {
    println("Already analyzed");
    return;
  }
  println(i, v[i]);
  v[i].mark(1);
  if (v[i].iscontainer()) {
    println("This value is a container...");
    traversal(v[i], 0);
  }
  traversal(v, i + 1);
}

// We mark our vector to avoid traversing it twice
v.mark(1);
traversal(v, 0);

// We unmark all elements at once
v.unmark();

// Output:
// [10,20,30,40,50,[60,70,80],[...]], it contains a self reference
// The run
// 0 10
// 1 20
// 2 30
// 3 40
// 4 50
// 5 [60,70,80]
// This value is a container...
// 0 60
// 1 70
// 2 80
// End of vector
// Already analyzed
// end
```