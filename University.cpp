#include "University.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

using namespace std;

Student::Student(string studentId, string studentName, double studentGpa)
    : id((studentId)), name((studentName)), gpa(studentGpa) {
    if (id.empty() || name.empty()) throw invalid_argument("Student ID and name cannot be empty");
    if (!isfinite(gpa) || gpa < 0 || gpa > 4) throw invalid_argument("GPA must be between 0 and 4");
}

Course::Course(string courseId, string courseName, shared_ptr<Professor> teacher)
    : id((courseId)), name((courseName)), professor((teacher)) {
    if (id.empty() || name.empty() || !professor) throw invalid_argument("Invalid course information");
}

void Course::enroll(shared_ptr<Student> student) {
    if (!student) throw invalid_argument("Student is missing");
    for (const auto& existing : students) {
        if (existing->id == student->id) throw invalid_argument("Student already enrolled: " + student->id);
    }
    students.push_back((student));
}

Department::Department(string departmentId, string departmentName)
    : id((departmentId)), name((departmentName)) {
    if (id.empty() || name.empty()) throw invalid_argument("Invalid department information");
}

void Department::addProfessor(shared_ptr<Professor> professor) {
    if (!professor) throw invalid_argument("Professor is missing");
    for (const auto& existing : professors) {
        if (existing->id == professor->id) throw invalid_argument("Professor already assigned");
    }
    professors.push_back((professor));
}

Course& Department::addCourse(string courseId, string courseName, shared_ptr<Professor> professor) {
    if (!professor) throw invalid_argument("Professor is missing");
    bool assigned = false;
    for (const auto& person : professors) {
        if (person->id == professor->id) assigned = true;
    }
    if (!assigned) throw invalid_argument("Professor must belong to this department");
    for (const auto& course : courses) {
        if (course->id == courseId) throw invalid_argument("Duplicate course: " + courseId);
    }
    courses.push_back(make_unique<Course>((courseId), (courseName), (professor)));
    return *courses.back();
}

Course& Department::findCourse(const string& courseId) {
    for (const auto& course : courses) {
        if (course->id == courseId) return *course;
    }
    throw out_of_range("Course not found: " + courseId);
}

shared_ptr<Student> University::addStudent(const string& id, const string& name, double gpa) {
    if (students.count(id)) throw invalid_argument("Duplicate student: " + id);
    auto student = make_shared<Student>(id, name, gpa);
    students[id] = student;
    return student;
}

shared_ptr<Professor> University::addProfessor(const string& id, const string& name) {
    if (id.empty() || name.empty()) throw invalid_argument("Invalid professor information");
    if (professors.count(id)) throw invalid_argument("Duplicate professor: " + id);
    auto professor = make_shared<Professor>(Professor{id, name});
    professors[id] = professor;
    return professor;
}

Department& University::addDepartment(const string& id, const string& name) {
    for (const auto& department : departments) {
        if (department->id == id) throw invalid_argument("Duplicate department: " + id);
    }
    departments.push_back(make_unique<Department>(id, name));
    return *departments.back();
}

Department& University::findDepartment(const string& id) {
    for (const auto& department : departments) {
        if (department->id == id) return *department;
    }
    throw out_of_range("Department not found: " + id);
}

shared_ptr<Student> University::findStudent(const string& id) const {
    auto it = students.find(id);
    if (it == students.end()) throw out_of_range("Student not found: " + id);
    return it->second;
}

shared_ptr<Professor> University::findProfessor(const string& id) const {
    auto it = professors.find(id);
    if (it == professors.end()) throw out_of_range("Professor not found: " + id);
    return it->second;
}

University makeDemo() {
    University university;
    auto russel = university.addProfessor("P01", "Dr. Russel");
    auto ambrosio = university.addProfessor("P02", "Dr. Ambrosio");
    auto lisa = university.addStudent("S01", "Lisa", 3.95);
    auto bart = university.addStudent("S02", "Bart", 2.45);
    auto maggie = university.addStudent("S03", "Maggie", 3.75);
    auto milhouse = university.addStudent("S04", "Milhouse", 3.25);

    auto& cs = university.addDepartment("CS", "Computer Science");
    cs.addProfessor(russel);
    cs.addProfessor(ambrosio);
    auto& data = cs.addCourse("CS201", "Data Structures", russel);
    auto& systems = cs.addCourse("CS310", "Operating Systems", ambrosio);

    for (auto student : {lisa, bart, maggie, milhouse}) data.enroll(student);
    for (auto student : {lisa, maggie}) systems.enroll(student);
    return university;
}

string trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

vector<string> split(const string& text) {
    vector<string> fields;
    string part;
    stringstream line(text);
    while (getline(line, part, ',')) fields.push_back(trim(part));
    if (!text.empty() && text.back() == ',') fields.push_back("");
    return fields;
}

University loadUniversity(istream& input) {
    University university;
    string line;
    size_t lineNumber = 0;

    while (getline(input, line)) {
        lineNumber++;
        line = trim(line);
        if (line.empty()) continue;

        try {
            auto fields = split(line);
            const string& type = fields.at(0);
            auto requireFields = [&](size_t count) {
                if (fields.size() != count) throw invalid_argument("Wrong number of fields for " + type);
            };

            if (type == "PROFESSOR") {
                requireFields(3);
                university.addProfessor(fields[1], fields[2]);
            } else if (type == "STUDENT") {
                requireFields(4);
                size_t used = 0;
                double gpa = stod(fields[3], &used);
                if (used != fields[3].size()) throw invalid_argument("Invalid GPA");
                university.addStudent(fields[1], fields[2], gpa);
            } else if (type == "DEPARTMENT") {
                requireFields(3);
                university.addDepartment(fields[1], fields[2]);
            } else if (type == "ASSIGN_PROF") {
                requireFields(3);
                university.findDepartment(fields[1]).addProfessor(university.findProfessor(fields[2]));
            } else if (type == "COURSE") {
                requireFields(5);
                university.findDepartment(fields[1]).addCourse(fields[2], fields[3], university.findProfessor(fields[4]));
            } else if (type == "ENROLL") {
                requireFields(4);
                university.findDepartment(fields[1]).findCourse(fields[2]).enroll(university.findStudent(fields[3]));
            } else {
                throw invalid_argument("Unknown record: " + type);
            }
        } catch (const exception& e) {
            throw runtime_error("Line " + to_string(lineNumber) + ": " + e.what());
        }
    }
    if (input.bad()) throw runtime_error("Error reading data");
    return university;
}
