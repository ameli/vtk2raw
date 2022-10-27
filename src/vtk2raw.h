/*
 * ============================================================================
 *
 *       Filename:  vtk2raw.h
 *
 *    Description:  Converts vtk files to raw files of arrays
 *
 *        Version:  1.0
 *        Created:  01/29/2016 01:54:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Siavash Ameli
 *   Organization:  University Of California, Berkeley
 *
 * ============================================================================
 */

#ifndef __vtk2raw_h
#define __vtk2raw_h

// =====================
// Foreward Declarations
// =====================

// Incomplete Declarations
class vtkPointData;
class vtkDataArray;
// class fstream;

// Complete declarations
#include <fstream>

// =====
// Types
// =====

enum InputFileType
{
    VTK = 0,
    VTI,
    VTP,
    VTU,
    NUMBER_OF_INPUT_FILE_TYPES
};

// ==========
// Prototypes
// ==========

void PrintUsage(char *ExecutableName);

void ReadDataSetWriteToOutput(
        char *InputFilename,
        char *OutputFilename,
        bool BinaryOutputFile);

InputFileType DetermineInputFileType(char *InputFilename);

void ReadVTKInputFileWriteToOutputFile(
        char *InputFilename,
        char *OutputFilename,
        bool BinaryOutputFile);

void ReadVTIInputFileWriteToOutputFile(
        char *InputFilename,
        char *OutputFilename,
        bool BinaryOutputFile);

void ReadVTPInputFileWriteToOutputFile(
        char *InputFilename,
        char *OutputFilename,
        bool BinaryOutputFile);

void ReadVTUInputFileWriteToOutputFile(
        char *InputFilename,
        char *OutputFilename,
        bool BinaryOutputFile);

void WriteArraysToOutputFile(
        vtkPointData *InputPointData,
        char *OutputFilename,
        bool BinaryOutputFile);

void OpenFile(
        char *OutputFilename,
        bool BinaryOutputFile,
        std::ofstream &OutputFile);

void WriteArraysToASCIIFile(
        std::ofstream &OutputFile,   // Output
        vtkDataArray **InputDataArrays,
        unsigned int NumberOfArrays,
        unsigned int *NumberOfComponentsInEachArray);

void WriteArraysToBinaryFile(
        std::ofstream &OutputFile,   // Output
        vtkDataArray **InputDataArrays,
        unsigned int NumberOfArrays,
        unsigned int *NumberOfComponentsInEachArray);

#endif
