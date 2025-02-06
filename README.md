# Regular Expression Matcher

A C implementation of Rob Pike's regular expression matcher with extended functionality and multi-position capabilities.

## Overview
This program implements a regular expression matcher that can find pattern matches within text and report their positions. It reads two lines from a file:
- **First line**: The regular expression pattern
- **Second line**: The text to search within

## Features
The matcher supports the following metacharacters:
- `.`  - Matches any single character
- `*`  - Matches zero or more occurrences of the previous character
- `?`  - Matches zero or one occurrence of the previous character
- `+`  - Matches one or more occurrences of the previous character
- `[]` - Matches any single character from the set
- `^`  - When used inside `[]`, negates the character set
- `\`  - Escapes special characters to match them literally

## Building
Compile the program using GCC:
```sh
gcc regular-expression-matcher.c -o regexmatcher
```

## Usage
Run the program by providing an input file:
```sh
./regexmatcher match.txt
```

### Input File Format
The input file should contain exactly two lines:
1. **First line**: The regular expression pattern
2. **Second line**: The text to search within

#### Example input file (`match.txt`):
```
[bd]
abcdefg 
```

### Output
The program outputs:
- `match n1 n2 n3 ...` where `n1, n2, n3` are the positions where matches were found (0-based indexing)
- `no match` if no matches are found

#### Example Usage:
```sh
$ cat match.txt
[bd]
abcdefg

$ ./regexmatcher match.txt
match 1
```

## Implementation Details
The program is structured into several key functions:
- `match`         - Main function that searches for pattern matches anywhere in the text
- `matchhere`     - Searches for pattern matches at the beginning of text
- `matchstar`     - Handles `*` metacharacter matching
- `matchquestion` - Handles `?` metacharacter matching
- `matchplus`     - Handles `+` metacharacter matching
- `matchset`      - Handles character set `[]` matching

## Limitations
- Maximum pattern and text length is **100 characters**
- Character sets **must be properly closed** with `]`
- **Nested character sets** are not supported

---
This implementation provides a simple yet powerful way to perform regex-based pattern matching with basic metacharacter support.