# Tamgu Grammar Library

The Tamgu Grammar Library is a powerful tool designed to assist coders in describing complex string structures. It provides a convenient way to detect specific substrings in a text, involving digits, uppercase letters, or punctuations in a strict order.

## Why use Tamgu Grammar?

- **Flexibility**: Tamgu Grammar allows you to define and manipulate complex string structures with ease.
- **Efficiency**: With Tamgu Grammar, you can efficiently detect and extract specific substrings from a text.
- **Ease of Use**: Tamgu Grammar provides a simple and intuitive interface, making it accessible to coders of all levels.

## Getting Started

To get started with Tamgu Grammar, follow these steps:

1. Install Tamgu on your system. You can find the installation instructions [here](https://github.com/naver/tamgu).
2. Import the Tamgu Grammar Library into your project.
3. Define your grammar rules using the provided functions and syntax.
4. Apply your grammar rules to the input text to detect and extract the desired substrings.

## Example Usage

Here's a simple example to illustrate the usage of Tamgu Grammar:

```python
from tamgu_grammar import Grammar

# Create a new grammar object
grammar = Grammar()

# Define a grammar rule to detect a sequence of digits followed by an uppercase letter
rule = grammar.rule()
rule.digit().one_or_more()
rule.uppercase_letter()

# Apply the grammar rules to the input text
result = grammar.apply("Hello, I have 3 Apples and 2 Oranges.")

# Print the detected substrings
for match in result:
    print(match)

# Output:
# 3A
# 2O
```

In this example, we create a grammar object and define a rule to detect a sequence of digits followed by an uppercase letter. We then apply the grammar rules to the input text and print the detected substrings.

## Documentation

For detailed documentation on how to use Tamgu Grammar, please refer to the [official documentation](https://github.com/naver/tamgu).

## Contributing

Contributions to the Tamgu Grammar Library are welcome! If you find any issues or have suggestions for improvement, please open an issue or submit a pull request on the [GitHub repository](https://github.com/naver/tamgu).

## License

Tamgu Grammar is licensed under the [MIT License](https://github.com/naver/tamgu/blob/master/LICENSE).