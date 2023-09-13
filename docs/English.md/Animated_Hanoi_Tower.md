## Animated Hanoi Tower

The code below displays an animation in which disks are moved from one column to another. It merges both graphics and predicates.

```prolog
% We declare our predicate
predicate move.

% The initial configuration... All disks are on the left column
map columns = { 'left': [70, 50, 30], 'centre': [], 'right': [] }.

% We draw a disk according to its position and its column
function disk(window w, int x, int y, int sz, int position) {
    int start = x + 100 - sz;
    int level = y - 50 * position;
    w.rectanglefill(start, level, sz * 2 + 20, 30, FL_BLUE);
}
```

## Analysis

The analysis:

```prolog
[sentence(['the', 'dog', 'eats', 'a', 'bat'], [], P(SN(dét(the), nom(dog)), SV(verbe(eats), SN(dét(a), nom(bat)))))]
```