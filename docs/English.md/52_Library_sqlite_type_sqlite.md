# Tamgu SQLite Library

Tamgu provides a simple library for handling SQLite databases. SQLite is a popular database system that uses simple files to execute SQL commands. If you need more information on SQLite, you can find plenty of resources on the web.

To use the SQLite library in Tamgu, you need to import it using the following command:

```python
use("sqlite")
```

## Methods

The SQLite library provides the following methods:

1. `begin()`: Enters the commit mode with DEFERRED mode.
2. `begin(string mode)`: Enters the commit mode with the specified mode (DEFERRED, EXCLUSIVE, IMMEDIATE).
3. `close()`: Closes the database.
4. `commit()`: Processes the SQL commands. This should be used to finish a series of commands initiated with `begin()`.
5. `create(x1, x2, x3)`: Creates a table in the database with the specified arguments.
6. `execute(string sqlcommand)`: Executes an SQL command without a callback.
7. `insert(table, column, value, ...)`: Inserts a line into a table.
8. `open(string pathname)`: Opens a database.
9. `run(string sqlcommand)`: Executes an SQL command with a callback to store results. If the input variable is a vector, all possible values will be stored in it. If the input variable is an iterator, it is possible to iterate over the results of the SQL command. Each result is a map where each key is a column name.

## Example

Here is an example of how to use the SQLite library:

```python
// Import the SQLite library
use("sqlite")

// Declare a new SQLite variable
sqlite mydb;

// Open a database. If it doesn't exist, it will be created
mydb.open('test.db');

try {
    // Create a new table in the current database
    mydb.create("table1", "nom TEXT PRIMARY KEY", "age INTEGER");
    println("table1 is created");
} catch () {
    // The database already exists
    println("Already created");
}

int i;
string nm;

// Insert values into the database using commit mode (faster)
mydb.begin();

// Insert 5000 elements
for (i = 0; i < 5000; i += 1) {
    nm = "tiia_" + i;
    try {
        // Insert two values into table1, one for 'nom' and the other for 'age'
        // Notice the alternation between column names and values
        mydb.insert("table1", "nom", nm, "age", i);
        println(i);
    } catch () {
        println("Already inserted");
    }
}

// Commit the commands
mydb.commit();

// Iterate over the values for a given SQL command
iterator it = mydb.run("select * from table1 where age > 10;");
for (it.begin(); it.end() == false; it.next()) {
    println("Value: ", it.value());
}

// Alternatively, we could have obtained the same result with:
// vector v = mydb.execute("select * from table1 where age > 10;");
// However, there is a risk of overflowing our vector, so it's not recommended.

// Close the database
mydb.close();
```