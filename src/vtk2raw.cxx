/*
 * ============================================================================
 *
 *       Filename:  vtk2raw.cpp
 *
 *    Description:  Converts vtk files to raw files of arrays
 *
 *        Version:  1.0
 *        Created:  01/28/2016 10:41:53 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Siavash Ameli
 *   Organization:  University Of California, Berkeley
 *
 * ============================================================================
 */

// Description:
//
// Input Files can be:
//   ASCII or binary   legacy VTK  file with   StructuredPoints
//   ASCII or binary   XML    VTI  file with   ImageData
//   ASCII or binary   XML    VTP  file with   PolyData
//   ASCII or binary   XML    VTU  file with   UnstructuredGrid
//
// Output file can be:
//   ASCII
//   Binary
//
// Contents to convert:
// The content of files that will be converted are ALL vtkDataArrays in the vtkPointData.
// The arrays can consist of:
//    All Scalar arrays
//    All Vector arrays
//    All Normal arrays
//    All Tensor arrays
//    All Field data arryas
//
// Each array may have different number of components.
// But all arrays should have the same number of tuples.
//
// The output is written to OutputFile as following:
// The output array is an array of size:
//     "Number of total components of all arrays" x "number of tuples".
//
//  where, the "Number of total components of all arrays" is the sum of number of components
//  of all arrays. If we have found three Arrays:
//
//  Array A: size a * m
//  Array B: size b * m
//  Array C: size c * m
//
//  Then the output array is of size (a+b+c) * m, and is written as follow
//
//              Column 1         Column 2         Column 3
//
//  First row:    A11 A12 A13 ...  B11 B12 B13 ...  C11 C12 C13 ...
//  Second row:   A21 A22 A23 ...  B21 B22 B23 ...  C21 C22 C23 ...
//                <-- a times -->  <-- b times -->  <-- c times -->
//                ...
//                ...
//  m-th row:     Am1 Am2 Am3 ...  Bm1 Bm2 Bm3 ...  Cm1 Cm2 Cm3 ...

// =======
// Headers
// =======

// Code
#include "vtk2raw.h"

// STL
#include <iomanip>     // for setprecision
#include <cstdlib>   // atio

// VTK
#include <vtkSmartPointer.h>
#include <vtkDataSet.h>
#include <vtkStructuredPointsReader.h>
#include <vtkXMLImageDataReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkStructuredPoints.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>

// ===========
// Definitions
// ===========

#define CHAR_LENGTH 256
#define DECIMAL_PRECISION 16

#define HERE std::cout << __FILE__ << " at line " << __LINE__ << std::endl;

// ====
// Main
// ====

int main(int argc, char *argv[])
{
    // Check arguments
    if(argc < 3)
    {
        PrintUsage(argv[0]);
        exit(0);
    }

    bool BinaryOutputFile = false;
    if(argc == 4)
    {
        int BinaryOutputFileInt = atoi(argv[3]);

        // Check input
        if(BinaryOutputFileInt != 0 && BinaryOutputFileInt != 1)
        {
            std::cerr << "Binary option should be either 0 or 1." << std::endl;
            exit(1);;
        }

        BinaryOutputFile = static_cast<bool>(BinaryOutputFileInt);
    }

    // Input/Output Filename
    char *InputFilename = argv[1];
    char *OutputFilename = argv[2];

    // Read DataSet and write to output file
    ReadDataSetWriteToOutput(InputFilename,OutputFilename,BinaryOutputFile);

    return EXIT_SUCCESS;
}

// ===========
// Print Usage
// ===========

void PrintUsage(char *ExecutableName)
{
    std::cerr << "Usage: " << ExecutableName;
    std::cerr << "  InputFileName.vtk  OutputFileName.raw  BinaryOutputFile";
    std::cerr << std::endl;
    std::cerr << "BinaryOutputFile is optional, it can be either 0 or 1.";
    std::cerr << std::endl;
}

// ============================
// Read DataSet Write To Output
// ============================

void ReadDataSetWriteToOutput(
        char *InputFilename,
        char *OutputFilename,
        bool BinaryOutputFile)
{
    // Determine input file type
    InputFileType InputFile =  DetermineInputFileType(InputFilename);

    // Read file
    if(InputFile == VTK)
    {
        ReadVTKInputFileWriteToOutputFile(
                InputFilename,
                OutputFilename,
                BinaryOutputFile);
    }
    else if(InputFile == VTI)
    {
        ReadVTIInputFileWriteToOutputFile(
                InputFilename,
                OutputFilename,
                BinaryOutputFile);
    }
    else if(InputFile == VTP)
    {
        ReadVTPInputFileWriteToOutputFile(
                InputFilename,
                OutputFilename,
                BinaryOutputFile);
    }
    else if(InputFile == VTU)
    {
        ReadVTUInputFileWriteToOutputFile(
                InputFilename,
                OutputFilename,
                BinaryOutputFile);
    }
    else
    {
        std::cerr << "Invalid input file type." << std::endl;
        exit(1);
    }
}

// =========================
// Determine Input File Type
// =========================

InputFileType DetermineInputFileType(char *InputFilename)
{
    // Get the file extension
    char FileExtension[CHAR_LENGTH];
    std::string InputFilenameString(InputFilename);
    std::size_t FoundLastDot = InputFilenameString.find_last_of(".");

    if(FoundLastDot != std::string::npos)
    {
        std::string FileExtensionString = InputFilenameString.substr(
                FoundLastDot+1,std::string::npos);
        strcpy(FileExtension,FileExtensionString.c_str());
    }
    else
    {
        std::cerr << "No file extension found in the Input filename.";
        std::cerr << std::endl;
        exit(1);
    }

    // Compare file extension
    if(strcmp(FileExtension,"vtk") == 0)
    {
        // VTK file
        return VTK;
    }
    else if(strcmp(FileExtension,"vti") == 0)
    {
        // VTI file
        return VTI;
    }
    else if(strcmp(FileExtension,"VTP"))
    {
        // VTP file
        return VTP;
    }
    else if(strcmp(FileExtension,"VTU"))
    {
        return VTU;
    }
    else
    {
        std::cerr << "No valid input file extension found." << std::endl;
        exit(1);
    }
}

// ========================================
// Read VTK Input File Write To Output File
// ========================================

// Description:
//
// Note that VTK legacy file readers such as
//    StructuredPointsReader,
//    StructuredGridReader,
//    UnstructuredGridReader,
//    ImageDataReader,
//    PolyDataReader
// Do not read all Scalars and vectors and Tensors. They only read the first
// scalar, the first vector, etc. To Read them all, we should add enable
// methods ReadAll...On.
//
// Note: For XML data (see next functions), these options do not have to be
// enabled, since XML readers will read all scalars, vectors, etc.
//
// This method assumes that the VTK legacy data file is StructuredPoints.

void ReadVTKInputFileWriteToOutputFile(
        char *InputFilename,
        char *OutputFilename,
        bool BinaryOutputFile)
{
    // Reader
    vtkSmartPointer<vtkStructuredPointsReader> StructuredPointsReader = \
            vtkSmartPointer<vtkStructuredPointsReader>::New();
    StructuredPointsReader->SetFileName(InputFilename);
    StructuredPointsReader->ReadAllScalarsOn();
    StructuredPointsReader->ReadAllVectorsOn();
    StructuredPointsReader->ReadAllNormalsOn();
    StructuredPointsReader->ReadAllTensorsOn();
    StructuredPointsReader->ReadAllFieldsOn();
    StructuredPointsReader->Update();

    // Get Point Data from loaded file
    vtkPointData *InputPointData = \
            StructuredPointsReader->GetOutput()->GetPointData();

    // WriteArraysToOutputFile(OutputFilename,ArraysInFile);
    WriteArraysToOutputFile(InputPointData,OutputFilename,BinaryOutputFile);
}

// ========================================
// Read VTI Input File Write To Output File
// ========================================

void ReadVTIInputFileWriteToOutputFile(
        char *InputFilename,
        char *OutputFilename,
        bool BinaryOutputFile)
{
    // Reader
    vtkSmartPointer<vtkXMLImageDataReader> ImageDataReader = \
            vtkSmartPointer<vtkXMLImageDataReader>::New();
    ImageDataReader->SetFileName(InputFilename);
    ImageDataReader->Update();

    // Get Point data from loaded file
    vtkPointData *InputPointData = \
            ImageDataReader->GetOutput()->GetPointData();

    // Write to output file
    WriteArraysToOutputFile(InputPointData,OutputFilename,BinaryOutputFile);
}

// ========================================
// Read VTP Input File Write To Output File
// ========================================

void ReadVTPInputFileWriteToOutputFile(
        char *InputFilename,
        char *OutputFilename,
        bool BinaryOutputFile)
{
    // Reader
    vtkSmartPointer<vtkXMLPolyDataReader> PolyDataReader = \
            vtkSmartPointer<vtkXMLPolyDataReader>::New();
    PolyDataReader->SetFileName(InputFilename);
    PolyDataReader->Update();

    // Get Point Data from loaded file
    vtkPointData *InputPointData = \
            PolyDataReader->GetOutput()->GetPointData();

    // Write to output file
    WriteArraysToOutputFile(InputPointData,OutputFilename,BinaryOutputFile);
}

// ========================================
// Read VTU Input File Write To Output File
// ========================================

void ReadVTUInputFileWriteToOutputFile(
        char *InputFilename,
        char *OutputFilename,
        bool BinaryOutputFile)
{
    // Reader
    vtkSmartPointer<vtkXMLUnstructuredGridReader> UnstructuredGridReader = \
            vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
    UnstructuredGridReader->SetFileName(InputFilename);
    UnstructuredGridReader->Update();

    // Get Point data from loaded file
    vtkPointData *InputPointData = \
            UnstructuredGridReader->GetOutput()->GetPointData();

    // Write to output file
    WriteArraysToOutputFile(InputPointData,OutputFilename,BinaryOutputFile);
}

// ===========================
// Write Arrays To Output File
// ===========================

// Description:
// This method finds the number of arrays in the PointData of the DataSet.
// - Each array may have different number of components.
// - But all arrays should have the same number of tuples.
//
// The output is written to OutputFile as following:
// The output array is an array of size:
//     "Number of total components of all arrays" x "number of tuples".
//
//  where, the "Number of total components of all arrays" is the sum of number
//  of components of all arrays. If we have found three Arrays:
//
//  Array A: size a * m
//  Array B: size b * m
//  Array C: size c * m
//
//  Then the output array is of size (a+b+c) * m, and is written as follow
//
//              Column 1         Column 2         Column 3
//
//  First row:    A11 A12 A13 ...  B11 B12 B13 ...  C11 C12 C13 ...
//  Second row:   A21 A22 A23 ...  B21 B22 B23 ...  C21 C22 C23 ...
//                <-- a times -->  <-- b times -->  <-- c times -->
//                ...
//                ...
//  m-th row:     Am1 Am2 Am3 ...  Bm1 Bm2 Bm3 ...  Cm1 Cm2 Cm3 ...

void WriteArraysToOutputFile(
        vtkPointData *InputPointData,
        char *OutputFilename,
        bool BinaryOutputFile)
{
    // Find number of arrays and their components
    int NumberOfArrays = InputPointData->GetNumberOfArrays();

    // Check for empty arrays
    if(NumberOfArrays < 1)
    {
        std::cerr << "DataSet has no array." << std::endl;
        exit(1);
    }

    // Number of components of each of the arrays
    unsigned int NumberOfComponentsInEachArray[NumberOfArrays];
    unsigned int NumberOfTuplesInEachArray[NumberOfArrays];
    unsigned int ColumnCounter = 0;

    // Create Array for pointing to the each of the data arrays
    vtkDataArray *InputDataArrays[NumberOfArrays];
    for(unsigned int ArrayIterator = 0;
        ArrayIterator < NumberOfArrays;
        ArrayIterator++)
    {
        // Get point to array
        InputDataArrays[ArrayIterator] = \
                InputPointData->GetArray(ArrayIterator);

        // Check array
        if(InputDataArrays[ArrayIterator] == NULL)
        {
            std::cerr << "Array " << ArrayIterator << " is NULL." << std::endl;
        }

        // Get number of components
        NumberOfComponentsInEachArray[ArrayIterator] = \
                InputDataArrays[ArrayIterator]->GetNumberOfComponents();
        NumberOfTuplesInEachArray[ArrayIterator] = \
                InputDataArrays[ArrayIterator]->GetNumberOfTuples();

        // Check tuples
        if(ArrayIterator > 0)
        {
            if(NumberOfTuplesInEachArray[ArrayIterator] != \
                NumberOfTuplesInEachArray[0])
            {
                std::cerr << "Inconsistent file: ";
                std::cerr << "number of tuples in arrays are not the same.";
                std::cerr << std::endl;
                exit(1);
            }
        }

        // Update total number of columns
        ColumnCounter += NumberOfComponentsInEachArray[ArrayIterator];

        std::cout << "Array: " << ArrayIterator << ", NumberOfComponents: ";
        std::cout << NumberOfComponentsInEachArray[ArrayIterator];
        std::cout << ", NumberOfTuples: ";
        std::cout << NumberOfTuplesInEachArray[ArrayIterator];
        std::cout << ", ArrayName: ";
        std::cout << InputDataArrays[ArrayIterator]->GetName();
        std::cout << std::endl;
    }

    // Open output file
    std::ofstream OutputFile;
    OpenFile(OutputFilename,BinaryOutputFile,OutputFile);

    // Write to ASCII or Binary
    if(BinaryOutputFile == false)
    {
        // Write to ASCII file
        WriteArraysToASCIIFile(
                OutputFile,   // Output
                InputDataArrays,
                NumberOfArrays,
                NumberOfComponentsInEachArray);
    }
    else
    {
        // Write to Binary file
        WriteArraysToBinaryFile(
                OutputFile,   // Output
                InputDataArrays,
                NumberOfArrays,
                NumberOfComponentsInEachArray);
    }

    std::cout << NumberOfArrays;
    std::cout << " arrays in column-wise order as above were written to: ";
    std::cout << OutputFilename << "." << std::endl;
    std::cout << "Rows: " << NumberOfTuplesInEachArray[0] << ", Columns: ";
    std::cout << ColumnCounter << "." << std::endl;

    // Close file
    OutputFile.close();
}

// =========
// Open File
// =========

void OpenFile(
        char *OutputFilename,
        bool BinaryOutputFile,
        std::ofstream &OutputFile)
{
    // Open ascii or binary
    if(BinaryOutputFile == false)
    {
        // Open ASCII file
        OutputFile.open(OutputFilename);
    }
    else
    {
        // Open Binary file
        OutputFile.open(OutputFilename,std::ios::binary);
    }

    // Check file is open
    if(OutputFile.is_open() != true)
    {
        std::cerr << "Can not open output file: ";
        std::cerr << OutputFilename << std::endl;
        exit(1);
    }

    OutputFile << std::setprecision(DECIMAL_PRECISION);
}

// ==========================
// Write Arrays To ASCII File
// ==========================

// Description:
// In ASCII mode, columns are separated by a delimiter, such as a tab.
// The rows are separated by new line.

void WriteArraysToASCIIFile(
        std::ofstream &OutputFile,   // Output
        vtkDataArray **InputDataArrays,
        unsigned int NumberOfArrays,
        unsigned int *NumberOfComponentsInEachArray)
{
    std::cout << "Write to binary file." << std::endl;

    std::string Delimiter("\t");

    // Number of Tuples
    unsigned long long int NumberOfTuples = \
            InputDataArrays[0]->GetNumberOfTuples();

    // Iterate over tuples
    for(unsigned long long int TupleIterator = 0;
        TupleIterator < NumberOfTuples;
        TupleIterator++)
    {
        // Iterate over arrays
        for(unsigned int ArrayIterator = 0;
            ArrayIterator < NumberOfArrays;
            ArrayIterator++)
        {
            // Iterate over components of each array
            for(unsigned int ComponentIterator = 0;
                ComponentIterator < \
                    NumberOfComponentsInEachArray[ArrayIterator];
                ComponentIterator++)
            {
                // Write to ASCII file
                OutputFile << InputDataArrays[ArrayIterator]->GetComponent(
                        TupleIterator,ComponentIterator);

                // Insert delimiter berween components
                if(ComponentIterator < \
                        NumberOfComponentsInEachArray[ArrayIterator]-1)
                {
                    OutputFile << Delimiter;
                }
            }

            // Insert delimiter between arrays
            if(ArrayIterator < NumberOfArrays-1)
            {
                OutputFile << Delimiter;
            }
        }

        // Insert new line
        if(TupleIterator < NumberOfTuples -1)
        {
            OutputFile << std::endl;
        }
    }
}

// ===========================
// Write Arrays To Binary File
// ===========================

// Description:
// 
// In Binary mode, there is no delimiter or new line netween rows and columns.
// Indeed there is no columsn and rows. All data are stored consequently on the
// memory. The application code that reads the output file should be aware of
// how to read the chunk of bytes.
//
// Data are stored as double. The number of components (columns) for each array
// is printed afterward to the screen. 
//
// To view the binary file in terminal in ASCII mode, use
//
//    $ hexdump -v -e '7/8 "%f "' -e '"\n"' OutputFile.raw
//
// In above, 8 is for double precision. FOr float, replace 8 with 4.
// In above, the file is printed in 7 columns. Change 7 to the number of
// columns.

void WriteArraysToBinaryFile(
        std::ofstream &OutputFile,   // Output
        vtkDataArray **InputDataArrays,
        unsigned int NumberOfArrays,
        unsigned int *NumberOfComponentsInEachArray)
{
    std::cout << "Write to binary file." << std::endl;

    // Number of Tuples
    unsigned long long int NumberOfTuples = \
            InputDataArrays[0]->GetNumberOfTuples();

    // Iterate over tuples
    for(unsigned long long int TupleIterator = 0;
        TupleIterator < NumberOfTuples;
        TupleIterator++)
    {
        // Iterate over arrays
        for(unsigned int ArrayIterator = 0;
            ArrayIterator < NumberOfArrays;
            ArrayIterator++)
        {
            // Iterate over components of each array
            for(unsigned int ComponentIterator = 0;
                ComponentIterator < \
                    NumberOfComponentsInEachArray[ArrayIterator];
                ComponentIterator++)
            {
                // Write to ASCII file
                double Value = InputDataArrays[ArrayIterator]->GetComponent(
                        TupleIterator,ComponentIterator);
                OutputFile.write(
                        reinterpret_cast<char*>(&Value),sizeof(Value));
            }
        }
    }
}
