# clarge

Analyse recursively given directory and print out files a least 1GiB of size.
The idea here is to find out which files starve your data storage.
The goal was to reproduce in C my golang based CLI [golarge](https://github.com/will666/golarge).

## Build

Build optimized binary:
``` shell
make prod
```

Build unoptimized binary with debug symbols:
``` shell
make
```

Build and run unoptimized binary with directory path argument:
``` shell
make run DIR=/var
```

Build optimized static binary:
``` shell
make static
```

Clean up build files in project:
``` shell
make clean
```

## Usage

``clarge [OPTIONS...] <PATH>``

### Flags:

``-s`` minimum target file size in bytes (default: 1073741824 bytes = 1GiB)

``-o`` save results to file

``-v`` print warning messages instead of logging to file

``-h`` print this help and exit

### Examples:

Analyse "/var" and save results to file:

``` shell
./clarge -o /var
```

Analyse "/usr" printing out warnings and result:

``` shell
./clarge -v /usr
```

## Limitations

Should compile and run on POSIX platforms, tested on GNU/Linux and Darwin (MacOS).

## License

[MIT](LICENSE)

## Credits

Author: [William Valentin](https://github.com/will666).

Use of pieces of code by [Alexey Kutepov](https://github.com/tsoding).