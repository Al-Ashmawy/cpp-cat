# Cat Implementation in C++

A clean, modern C++ implementation of the Unix `cat` utility, matching GNU coreutils behavior.

## Overview

This project reimplements the `cat` command from scratch in C++, focusing on:
- Clean architecture and readable code
- POSIX compliance
- Comprehensive flag support
- Proper error handling

## Features

### Supported Options

| Flag | Long Form | Description |
|------|-----------|-------------|
| `-n` | `--number` | Number all output lines |
| `-b` | `--number-nonblank` | Number only non-empty lines (overrides `-n`) |
| `-E` | `--show-ends` | Display `$` at end of each line |
| `-T` | `--show-tabs` | Display TAB characters as `^I` |
| `-s` | `--squeeze-blank` | Suppress repeated empty lines |
| `-v` | `--show-nonprinting` | Use `^` and `M-` notation for special characters |
| `-A` | `--show-all` | Equivalent to `-vET` |
| `-e` | - | Equivalent to `-vE` |
| `-t` | - | Equivalent to `-vT` |
| `-u` | - | Ignored (POSIX compatibility) |
| - | `--help` | Show help message |
| - | `--version` | Show version information |

### Combined Options

Short options can be combined:
```bash
./cat -nv file.txt    # Same as ./cat -n -v file.txt
./cat -et file.txt    # Same as ./cat -e -t file.txt
```

### Input Modes

- **Read files:** `./cat file1.txt file2.txt`
- **Read stdin:** `./cat` or `./cat -`
- **Mix both:** `./cat file1.txt - file2.txt` (reads file1, then stdin, then file2)

## Building

### Requirements

- C++23 or later
- Standard C++ library

### Compilation

```bash
g++ -std=c++23 cat.cpp -o cat
```

Or with clang:
```bash
clang++ -std=c++23 cat.cpp -o cat
```

## Usage Examples

### Basic usage

```bash
# Print file contents
./cat file.txt

# Concatenate multiple files
./cat file1.txt file2.txt file3.txt

# Read from stdin
echo "hello world" | ./cat

# Read from multiple sources
./cat file1.txt - file2.txt  # file1, then stdin, then file2
```

### Number lines

```bash
# Number all lines
./cat -n file.txt

# Number only non-empty lines
./cat -b file.txt
```

### Show special characters

```bash
# Show tabs as ^I
./cat -T file.txt

# Show line endings with $
./cat -E file.txt

# Show all special characters (equivalent to -vET)
./cat -A file.txt

# Show control characters and line endings
./cat -e file.txt  # equivalent to -vE
```

### Combine options

```bash
# Number lines and show tabs
./cat -nT file.txt

# Number non-empty lines, show all special chars
./cat -bA file.txt

# Squeeze blank lines and show endings
./cat -sE file.txt
```

### Squeeze blank lines

```bash
# Remove repeated empty lines
./cat -s file.txt
```

## Resources

- [GNU Coreutils cat](https://www.gnu.org/software/coreutils/manual/html_node/cat-invocation.html)
- C++ Standard Library reference
