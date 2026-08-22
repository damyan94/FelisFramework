# Felis example

The example is a small text-statistics CLI application. It demonstrates the
`Felis::Application` lifecycle, command-line argument handling, logging, file I/O and
application exit codes without introducing additional dependencies.

## Building

The project CMake presets enable the example by default. Platform-specific build
and execution instructions are available in:

- [Linux build, run and debug](../Docs/LINUX_BUILD_RUN_DEBUG.md)
- [Android build, run and debug](../Docs/ANDROID_BUILD_RUN_DEBUG.md)

The executable is produced under the selected platform build directory in:

```text
Example/FelisExample
```

## Usage

```text
FelisExample [--mode=lines|words|bytes] [--verbose] [--log-file=<file>] <file>
```

Options:

- `--mode=lines` counts logical text lines and is the default;
- `--mode=words` counts whitespace-separated words;
- `--mode=bytes` counts bytes read from the file;
- `--verbose` enables informational logging;
- `--log-file=FILE` appends plain-text logs to `FILE`;
- `--help` prints usage information;
- `--` stops named-argument parsing.

Examples:

```bash
Build/Linux/Debug/Example/FelisExample --mode=lines README.md
Build/Linux/Debug/Example/FelisExample --mode=words --verbose README.md
Build/Linux/Debug/Example/FelisExample --mode=words --verbose --log-file=FelisExample.log README.md
Build/Linux/Debug/Example/FelisExample --help
```

Cross-compiled executables must be deployed to and run on their target
platform, as described in the corresponding platform guide.

Exactly one input file is required unless `--help` is present. The line count
includes a final non-empty line when the file does not end with a newline.

Invalid arguments return `InvalidCommandLineArguments`. File opening or reading
failures return `RuntimeFailed`; contextual details are written through the
logger.
