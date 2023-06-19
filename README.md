# Implementation of the Shamir Shared Secret algorithm for black and white images

## Contents:
1. [Authors](#1-authors)
1. [Prerrequisites](#2-prerrequisites)
1. [Compilation](#3-compilation)
1. [Execution](#4-execution)
1. [Cleanup](#5-cleanup)

### 1. Authors
* [Chao, Florencia](https://github.com/florchao)
* [Cornidez, Milagros](https://github.com/mcornidez)
* [De Luca, Juan Manuel](https://github.com/juandl14)
* [Konfederak, Sol](https://github.com/solkonfe)

### 2. Prerrequisites
In order for the program with the recover option to work, one must have previously created the output file with .bmp extension and assigned the correct permissions to it.

### 3. Compilation
To compile, after cloning the repo, run the following command at the root level of the repo:
```
$> make all
```

### 4. Execution
In the case that one wants to distribute a secret between images, run the following command:
```
$> ./ss d [PATH_TO_SECRET] 3 images
```
where PATH_TO_SECRET is the path to a .bmp image in black and white (8 bits per pixel) that you want distributed between the files of the images directory.

In the case that one wants to recover a secret, run the following comand:
```
$> ./ss r out.bmp 3 images
```
where out.bmp is a previously created .bmp file with the correct permissions, in which the recovered image will be formed.

### 5. Cleanup
To delete all executables and target files created in the compilation and execution, run the following command in the roor directory of the repo:
```
$> make clean
```