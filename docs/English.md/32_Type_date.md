## Date Type

The `Date` type is used to handle dates.

### Methods

1. `date()`: Returns the date as a string.
2. `day()`: Returns the day as an integer.
3. `format(string f)`: Returns the formatted date as a string. The format string uses a combination of options. See below for an explanation.
4. `hour()`: Returns the hour as an integer.
5. `min()`: Returns the minute as an integer.
6. `month()`: Returns the month as an integer.
7. `sec()`: Returns the second as an integer.
8. `setdate(year, month, day, hour, min, sec)`: Sets the date and time variables.
9. `year()`: Returns the year as an integer.
10. `yearday()`: Returns the year day as an integer between 0-365.
11. `weekday()`: Returns the week day as an integer between 0-6, where 0 represents Sunday.

### Operators

- `+` and `-`: Dates can be added or subtracted.

### Format Options

- `%a`: The abbreviated weekday name according to the current locale.
- `%A`: The full weekday name according to the current locale.
- `%b`: The abbreviated month name according to the current locale.
- `%B`: The full month name according to the current locale.
- `%c`: The preferred date and time representation for the current locale.
- `%C`: The century number (year/100) as a 2-digit integer.
- `%d`: The day of the month as a decimal number (range 01 to 31).
- `%D`: Equivalent to `%m/%d/%y`.
- `%e`: Like `%d`, the day of the month as a decimal number, but a leading zero is replaced by a space.
- `%F`: Equivalent to `%Y-%m-%d` (the ISO 8601 date format).
- `%G`: The ISO 8601 week-based year with century as a decimal number.
- `%g`: Like `%G`, but without century, that is, with a 2-digit year (00-99).
- `%h`: Equivalent to `%b`.
- `%H`: The hour as a decimal number using a 24-hour clock (range 00 to 23).
- `%I`: The hour as a decimal number using a 12-hour clock (range 01 to 12).
- `%j`: The day of the year as a decimal number (range 001 to 366).
- `%k`: The hour (24-hour clock) as a decimal number (range 0 to 23); single digits are preceded by a blank.
- `%l`: The hour (12-hour clock) as a decimal number (range 1 to 12); single digits are preceded by a blank.
- `%m`: The month as a decimal number (range 01 to 12).
- `%M`: The minute as a decimal number (range 00 to 59).
- `%n`: A newline character.
- `%p`: Either "AM" or "PM" according to the given time value.
- `%P`: Like `%p` but in lowercase.
- `%r`: The time in a.m. or p.m. notation.
- `%R`: The time in 24-hour notation.
- `%s`: The number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
- `%S`: The second as a decimal number (range 00 to 60).
- `%t`: A tab character.
- `%T`: The time in 24-hour notation.
- `%u`: The day of the week as a decimal, range 1 to 7, Monday being 1.
- `%U`: The week number of the current year as a decimal number, range 00 to 53, starting with the first Sunday as the first day of week 01.
- `%V`: The ISO 8601 week number of the current year as a decimal number, range 01 to 53, where week 1 is the first week that has at least 4 days in the new year.
- `%w`: The day of the week as a decimal, range 0 to 6, Sunday being 0.
- `%W`: The week number of the current year as a decimal number, range 00 to 53, starting with the first Monday as the first day of week 01.
- `%x`: The preferred date representation for the current locale without the time.
- `%X`: The preferred time representation for the current locale without the date.
- `%y`: The year as a decimal number without a century (range 00 to 99).
- `%Y`: The year as a decimal number including the century.
- `%z`: The numeric timezone (hour and minute offset from UTC).
- `%Z`: The timezone or name or abbreviation.
- `%+`: The date and time in date format.
- `%%`: A literal '%' character.

### Examples

Example 1:

```python
Date d;
println(d.format("%Y%m%d")); // Display the date for 2015/12/25 as 20151225
```

Example 2:

```python
Date mytime;
print(mytime); // Display: 2010/07/08 15:19:22
```