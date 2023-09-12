# Pause and Sleep

These two functions are used to either pause or put a thread to sleep.

`pause` does not suspend the execution of a thread, while `sleep` does.

`pause` takes a float value as input, representing the number of seconds to pause. It can also take a second Boolean parameter to display a small animation.

`sleep` is based on the OS sleep instruction and its behavior depends on the local implementation. It takes an integer value as input.

Example:

```python
pause(0.1)  # The thread will pause for 10 ms
pause(2, True)  # The thread will pause for 2s, with a small animation
sleep(1)  # The thread will sleep for 1s (depending on the platform)
```