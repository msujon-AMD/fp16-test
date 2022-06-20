# fp16-test

## dependencies

hip header files (INCDIR=/opt/rocm/include/hip)

## build 
```
$ make 
```
## run
```
$ ./xtstf16 [OPTIONS] 
```
Example: 

```
$./xtstf16  --print-number-table
$ ./xtstf16 --print-number-table --skip-non-normal
$ ./xtstf16 --print-number-table --skip-non-normal --output filename
$ ./xtstf8 --help

Usage :
--no-skip <1/0>            : 1-> print all numbers, 0-> skip denorm, +/-nan, +/-inf, +/-0.0 
--print-number-table       : print all reprsentable numbers in table with header (default)
--epsilon                  : print epsilon 
--outfile <filename>       : output filename
--help                     : print usage

```
