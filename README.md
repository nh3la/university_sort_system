University Management System using C++

This project is a C++ University Management System that organizes students, professors, departments, and courses. It uses a custom merge sort algorithm to rank students by GPA and multithreading to generate course reports efficiently. It demonstrates object-oriented programming, smart pointers, file handling, error handling, and algorithm performance testing.

- University.hpp defines the Student, Professor, Course, Department, and University classes and their relationships. shared_ptr allows courses to reference students and professors, while unique_ptr manages ownership of courses and departments.
- University.cpp implements adding and finding records, enrolling students, and validating data. It also reads university data from a comma separated text file and reports errors, including invalid GPAs and duplicate enrollments.
- MergeSort.hpp implements a reusable template-based merge sort. It recursively splits a list, sorts each half, and merges them while counting comparisons and merge operations.
- main.cpp connects the features. It sorts students by highest GPA (then name), creates a report for each course, and uses std::async to process up to four reports concurrently. It also provides testing and benchmarking modes.
- sample.univ contains example professors, students, departments, courses, and enrollments for file import.

Run the Project

Compile the two .cpp files

bash
g++ -std=c++17 -pthread main.cpp University.cpp -o university_app

Choose a mode:

bash

./university_app demo                # Generate sample reports

./university_app import sample.univ  # Load data from a file

./university_app test                # Run five built-in tests

./university_app benchmark           # Measure merge sort speed and correctness
