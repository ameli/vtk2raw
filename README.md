# VTK to Binary

This code converts an ascii VTK file to a raw binary file.

**Input File:**

The input file can be either of the followings:

| Data Type       | File Structure | File Extension | Data Structure   |
| --------------- | -------------- | -------------- | ---------------- |
| ASCII or binary | legacy         | ``VTK``        | StructuredPoints |
| ASCII or binary | XML            | ``VTI``        | ImageData        |
| ASCII or binary | XML            | ``VTP``        | PolyData         |
| ASCII or binary | XML            | ``VTU``        | UnstructuredGrid |

**Output file:**

The output file has ``*.raw`` file extension and can be stored as either an *ASCII* file or a *binary* file.

**Converted Arrays:**

The content of files that will be converted are all ``vtkDataArrays`` in the ``vtkPointData``. The arrays can consist of:

* All scalar arrays
* All vector arrays
* All normal arrays
* All tensor arrays
* All field data arrays.

Each array may has different number of components. But all arrays should have the same number of tuples.

**Output Arrays:**

The output is written to the output file as following:

The output array is an array of size:

    "Number of total components of all arrays" x "number of tuples".

where, the "Number of total components of all arrays" is the sum of number of components of all arrays. If we have found three Arrays:

    Array A: size a * m
    Array B: size b * m
    Array C: size c * m

Then the output array is of size ``(a+b+c) * m``, and is written as follow
:
                  Column 1         Column 2         Column 3

    First row:    A11 A12 A13 ...  B11 B12 B13 ...  C11 C12 C13 ...
    Second row:   A21 A22 A23 ...  B21 B22 B23 ...  C21 C22 C23 ...
                  <-- a times -->  <-- b times -->  <-- c times -->
                  ...
                  ...
    m-th row:     Am1 Am2 Am3 ...  Bm1 Bm2 Bm3 ...  Cm1 Cm2 Cm3 ...

## Prerequisites

1. Install *GNU GCC* compiler, *OpenMP*, and *Cmake*:

       sudo apt-get install g++ libomp-dev cmake -y

2. Install *VTK* development:

       sudo apt install libvtk5-dev -y

   Alternatively, [download VTK](https://vtk.org/download/) and compile it.

## Install

Get the source code

    git clone git@github.com:ameli/vtk2raw.git
    cd vtk2raw

Compile in ``./build`` directory

    mkdir build
    cmake -B build .
    make -C build

After the compilation, the executable file is located in ``./bin/vtk2raw``.

## Usage

    ./bin/vtk2raw  InputFileName.vtk  OutputFileName.raw  <BinaryOutputFile>

The argument ``BinaryOutputFile`` is optional, it can be either ``0`` or ``1`` to indicate whether the output file should be ASCII or binary, respectively.

## License

BSD 3-clause.
