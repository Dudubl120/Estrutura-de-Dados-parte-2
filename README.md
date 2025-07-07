# Hospital Patient Management System - Data Structures Project Part 2

## Overview
This is a Hospital Patient Management System implemented in C using dynamic data structures. The system manages patient records with CRUD (Create, Read, Update, Delete) operations and persists data to a CSV file.

## Features
- **Patient Consultation**: Search patients by name or CPF
- **Patient Update**: Modify existing patient records
- **Patient Removal**: Delete patient records with confirmation
- **Patient Addition**: Add new patients with automatic ID assignment
- **List All Patients**: Display all registered patients
- **Data Persistence**: Automatic save to CSV file when exiting
- **Clear Terminal**: Clear screen functionality

## Data Structures Used
- **Dynamic Vector**: Implements a resizable array that automatically grows when needed
- **Linked List**: Each patient record is stored as a doubly-linked list of heterogeneous fields
- **Abstract Data Types**: The main program only uses function interfaces, keeping implementation details hidden

## Architecture
The system follows a modular design with clear separation of concerns:

### Files Structure
- `main.c`: User interface and main program logic
- `dinamic_vector.h/c`: Dynamic vector implementation
- `linkedlist.h/c`: Linked list implementation for heterogeneous data
- `bd_paciente.csv`: Patient database file
- `Makefile`: Build configuration

### Data Model
Each patient record contains:
- **ID**: Unique identifier (integer)
- **CPF**: Brazilian tax ID (string)
- **Nome**: Patient name (string)
- **Idade**: Age (integer)
- **Data_Cadastro**: Registration date (string, YYYY-MM-DD format)

## Fixed Issues
✅ **Function Documentation**: Added comprehensive English documentation for all functions
✅ **Variable Names**: Converted Portuguese variable names to English (e.g., `dv` → `patient_db`, `flag` → `user_choice`)
✅ **Data Persistence**: Implemented automatic save to CSV when exiting the program
✅ **Add Patient Feature**: Fixed registration date input - now allows manual date entry
✅ **Clear Terminal**: Fixed terminal clearing to work properly on Windows without exiting
✅ **Code Abstraction**: Ensured main.c only calls interface functions, hiding implementation details
✅ **Menu Options**: Simplified and clarified CRUD operation naming in code

## Compilation
```bash
gcc -o hospital_system main.c linkedlist.c dinamic_vector.c
```

Or use the provided Makefile:
```bash
make
```

## Usage
Run the executable and follow the menu prompts:
```bash
./hospital_system
```

The system will automatically load existing patient data from `bd_paciente.csv` and save any changes when you exit with 'Q'.

## Technical Details

### Memory Management
- All dynamic memory is properly allocated and freed
- String fields use `strdup()` for safe copying
- Comprehensive error handling for memory allocation failures

### Error Handling
- Input validation for menu options
- Bounds checking for array access
- File I/O error handling
- Memory allocation failure handling

### Performance Considerations
- Dynamic vector doubles in size when capacity is reached
- O(1) insertion at the end of dynamic vector
- O(n) search operations for patient consultation
- Efficient memory usage with automatic cleanup

