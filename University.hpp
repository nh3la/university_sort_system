#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Student {
    string id;
    string name;
    double gpa;

    Student(string id, string name, double gpa);
};

struct Professor {
    string id;
    string name;
};

class Course {
public:
    string id;
    string name;
    shared_ptr<Professor> professor;
    vector<shared_ptr<Student>> students;

    Course(string id, string name, shared_ptr<Professor> professor);
    void enroll(shared_ptr<Student> student);
};

class Department {
public:
    string id;
    string name;
    vector<shared_ptr<Professor>> professors;
    vector<unique_ptr<Course>> courses;

    Department(string id, string name);
    void addProfessor(shared_ptr<Professor> professor);
    Course& addCourse(string id, string name, shared_ptr<Professor> professor);
    Course& findCourse(const string& id);
};

class University {
public:
    unordered_map<string, shared_ptr<Student>> students;
    unordered_map<string, shared_ptr<Professor>> professors;
    vector<unique_ptr<Department>> departments;

    shared_ptr<Student> addStudent(const string& id, const string& name, double gpa);
    shared_ptr<Professor> addProfessor(const string& id, const string& name);
    Department& addDepartment(const string& id, const string& name);
    Department& findDepartment(const string& id);
    shared_ptr<Student> findStudent(const string& id) const;
    shared_ptr<Professor> findProfessor(const string& id) const;
};

University makeDemo();
University loadUniversity(istream& input);
