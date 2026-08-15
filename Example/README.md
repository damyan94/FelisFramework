# Felis example

The example is a small text-statistics CLI application. It demonstrates the
`Application` lifecycle, command-line argument handling, logging, file I/O and
application exit codes without introducing additional dependencies.

## Building

From the repository root:

```bash
cmake -S . -B Build -DFELIS_BUILD_EXAMPLE=ON
cmake --build Build -j4
```

## Usage

```text
FelisExample [--mode=lines|words|bytes] [--verbose] <file>
```

Options:

- `--mode=lines` counts logical text lines and is the default;
- `--mode=words` counts whitespace-separated words;
- `--mode=bytes` counts bytes read from the file;
- `--verbose` enables informational logging;
- `--help` prints usage information;
- `--` stops named-argument parsing.

Examples:

```bash
./Build/FelisExample --mode=lines README.md
./Build/FelisExample --mode=words --verbose README.md
./Build/FelisExample --help
```

Exactly one input file is required unless `--help` is present. The line count
includes a final non-empty line when the file does not end with a newline.

Invalid arguments return `InvalidCommandLineArguments`. File opening or reading
failures return `RuntimeFailed`; contextual details are written through the
logger.
