# C Employee Database

A simple C program to add, remove, update employees in a Database

## Usage

To use the Employee Database , run the executable with the following command-line options:

- `-n`: Create a new database file.
- `-f <filename>`: Specify the path to the database file.
- `-a "<name>, <address>, <hours>"`: Add a new employee.
- `-l`: List all employees in the database.
- `-r "<name>"`: Remove an employee by name.
- `-u "<name>" <hours>`: Update the hours worked for an employee.

## Execution

Go to the project folder and just type
```bash
make
```
You can change the Makefile to add or remove extra commands

## Project Structure

- **`src/`**: Contains the source code files.

  - **`file.c`**: File handling functions.
  - **`main.c`**: Main program logic.
  - **`parse.c`**: Parsing functions.

- **`include/`**: Holds header files.

  - **`common.h`**: Common definitions and macros.
  - **`file.h`**: File handling function declarations.
  - **`parse.h`**: Parsing function declarations.

- **`obj/`**: Houses object files generated during compilation.

- **`bin/`**: Stores the executable files.

- **`Makefile`**: The Makefile for building the project.

- **`README.md`**: Documentation providing an overview of the project, its features, and instructions for usage.

## License

This project is licensed under the [MIT License](https://choosealicense.com/licenses/mit/).

