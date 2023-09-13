## Autorun

The `autorun` function is automatically launched after it is declared. Autorun functions are only executed in the main file. If you have autorun functions in a file that is called from within another file, then these functions are not executed.

**Note:** Autorun functions are useless in frames.

### Example

```python
autorun waitOnRed() {
  println("Loading:", _paths[1]);
}
```