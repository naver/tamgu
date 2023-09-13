# Tamgu SQLite Library

Tamgu provides a simple library for handling SQLite databases. SQLite is a popular database system that uses simple files to execute SQL commands. If you need more information on SQLite, you can find plenty of resources on the web.

To use the SQLite library in Tamgu, you need to import it using the following command:

```python
use("sqlite")
```

## Methods

The SQLite library provides the following methods:

1. `begin()`: Enters the commit mode with DEFERRED mode.
2. `begin(string mode)`: Enters the commit mode with the specified mode (DEFERRED, EXCLUSIVE, or IMMEDIATE).
3. `close()`: Closes the database.
4. `commit()`: Processes the SQL commands. This should be called after a series of commands initiated with `begin()`.
5. `create(x1,x2,x3)`: Creates a table in the database with the specified arguments.
6. `execute(string sqlcommand)`: Executes an SQL command without a callback.
7. `insert(table,column,value,...)`: Inserts a row into a table.
8. `open(string pathname)`: Opens a database.
9. `run(string sqlcommand)`: Executes an SQL command with a callback to store the results. If the input variable is a vector, all possible values will be stored in it. If the input variable is an iterator, it is possible to iterate over the results of the SQL command. Each result is a map where each key is a column name.

## Example

Here is an example of how to use the SQLite library:

```python
use("sqlite")

sqlite mydb

# Open a database. If it doesn't exist, it will be created.
mydb.open('test.db')

# Perform operations on the database...
```

Please note that this is just a brief overview of the SQLite library in Tamgu. For more detailed information, please refer to the official documentation.