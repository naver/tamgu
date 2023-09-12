# wtable

The `Tamgu ltk` library provides a specific type to display values in a table and select some elements. This table must be created with a callback function, whose signature is as follows:

```lua
function callback_table(table x, map values, myobject obj)
```

To create the table, use the following syntax:

```lua
table t(obj) with callback_table
```

The `values` parameter is a map that contains the following keys:

- `"top"`: the top row
- `"bottom"`: the bottom row
- `"left"`: the left column
- `"right"`: the right column
- `"values"`: a map whose key is a string in the format "r:c", with `r` as the row and `c` as the column.

This object exposes the following methods:

## Methods

1. `add(int R, int C, string v)`: Add a value at row `R` and column `C`. The size of the table depends on the number of values added.
2. `boxtype(int boxtype)`: Set the box type.
3. `cell(int R, int C)`: Return the value at row `R` and column `C`.
4. `cellalign(align)`: Set the alignment in a cell.
5. `cellalignheaderrow(align)`: Set the alignment in a row header cell.
6. `cellalignheadercol(align)`: Set the alignment in a column header cell.
7. `clear()`: Clear the table.
8. `colorbg(int c)`: Set or return the cell background color.
9. `colorfg(int c)`: Set or return the cell foreground color.
10. `column()`: Return the number of columns.
11. `column(int nb)`: Define the number of columns.
12. `columnheader(int C, string label)`: Set the label of the column header for column `C`.
13. `columnheaderwidth(int sz)`: Set the size in pixels of the column header.
14. `columnwidth(int width)`: Define the column width in pixels.
15. `create(int x, int y, int w, int h, string label)`: Create a table of objects and start adding.
16. `font(int s)`: Set or return the text font.
17. `fontsize(int c)`: Set or return the text font size.
18. `row()`: Return the number of rows.
19. `row(int nb, int height)`: Define the number of rows.
20. `rowheader(int R, string label)`: Set the label of the row header for row `R`.
21. `rowheaderheight(int sz)`: Set the size in pixels of the row header.
22. `rowheight(int height)`: Define the row height in pixels.
23. `selectioncolor(int color)`: Set the color for the selected elements.
24. `when(string when)`: Define the type of event to trigger the callback.

## Example

```lua
use("gui")
window w

function callback_table(wtable x, map V, window w)
    println(V)
end

wtable t(w) with callback_table

int i, j

-- Create the window
w.begin(300, 200, 1300, 500, "Fenetre")

-- Create the table
t.create(20, 20, 500, 400, "table")

-- Set the font size
t.fontsize(12)

-- Set the selected element color to blue
t.selectioncolor(FL_BLUE)

-- Populate the table
for i = 0, 10 do
    -- Set the row headers
    t.rowheader(i, "R" .. i)
    -- Set the column headers
    t.columnheader(i, "C" .. i)
    for j = 0, 10 do
        -- Add values to the table
        t.add(i, j, "R" .. i .. "C" .. j)
    end
end

-- Set the row height after populating the table
t.rowheight(20)

-- Set the column width after populating the table
t.columnwidth(60)

w.end()
w.run()
```