# Date in Tamgu

This type is used to handle dates.

## Methods

1. `date()`: Returns the date as a string.
2. `day()`: Returns the day as an integer.
3. `format(string f)`: Returns the format as a string. The format string uses a combination of options. See below for an explanation.
4. `hour()`: Returns the hour as an integer.
5. `min()`: Returns the minute as an integer.
6. `month()`: Returns the month as an integer.
7. `sec()`: Returns the second as an integer.
8. `setdate(year, month, day, hour, min, sec)`: Sets a time variable.
9. `year()`: Returns the year as an integer.
10. `yearday()`: Returns the year day as an integer between 0-365.
11. `weekday()`: Returns the week day as an integer between 0-6. (0 is Sunday.)

## Operators

- `+`, `-`: Dates can be added or subtracted.

## Return Types

- As a string: Returns the date as a string.
- As an integer or a float: Returns the number of seconds elapsed since 00:00 hours, Jan 1, 1970 UTC.

## Format

- `%a`: The abbreviated weekday name according to the current locale.