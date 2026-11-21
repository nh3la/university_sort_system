#include "University.hpp"
#include "MergeSort.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>

using namespace std;

string makeReport(const string& departmentName, const Course& course) {
    vector<shared_ptr<Student>> roster = course.students;
    SortStats stats = mergeSort(roster, [](const auto& a, const auto& b) {
        if (a->gpa != b->gpa) return a->gpa > b->gpa;
        return a->name < b->name;
    });

    ostringstream out;
    out << "\n=== " << departmentName << " / " << course.name << " ===\n";
    out << "Professor: " << course.professor->name << "\n";
    for (const auto& student : roster) {
        out << "  " << student->id << " | " << left << setw(12) << student->name
            << " | GPA " << fixed << setprecision(2) << student->gpa << "\n";
    }
    out << "Students: " << roster.size() << " | Comparisons: " << stats.comparisons
        << " | Merges: " << stats.merges << "\n";
    return out.str();
}

vector<string> generateReports(const University& university, bool parallel = true) {
    vector<string> results;
    vector<future<string>> running;

    auto finishBatch = [&]() {
        for (auto& job : running) results.push_back(job.get());
        running.clear();
    };

    for (const auto& department : university.departments) {
        for (const auto& course : department->courses) {
            if (parallel) {
                string departmentName = department->name;
                const Course* current = course.get();
                running.push_back(async(launch::async, [departmentName, current]() {
                    return makeReport(departmentName, *current);
                }));
                if (running.size() == 4) finishBatch();
            } else {
                results.push_back(makeReport(department->name, *course));
            }
        }
    }
    finishBatch();
    return results;
}

void runBenchmark() {
    mt19937 random(42);
    uniform_int_distribution<int> distribution(0, 1000000);
    cout << "Size,Comparisons,Merges,Milliseconds,Correct\n";

    for (int size : {100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600}) {
        vector<int> numbers(size);
        for (int& number : numbers) number = distribution(random);
        vector<int> expected = numbers;
        sort(expected.begin(), expected.end());

        auto start = chrono::steady_clock::now();
        auto stats = mergeSort(numbers, less<int>{});
        auto stop = chrono::steady_clock::now();
        double ms = chrono::duration<double, milli>(stop - start).count();
        bool correct = (numbers == expected);
        cout << size << ',' << stats.comparisons << ',' << stats.merges << ','
             << fixed << setprecision(3) << ms << ',' << (correct ? "Yes" : "No") << '\n';
        if (!correct) throw runtime_error("Merge sort produced incorrect results");
    }
}

void check(bool condition, const string& message) {
    if (!condition) throw runtime_error("Test failed: " + message);
}

void runTests() {
    vector<int> values = {5, 1, 4, 1, 3};
    mergeSort(values, less<int>{});
    check(values == vector<int>({1, 1, 3, 4, 5}), "Sorting numbers");

    bool rejected = false;
    try { Student bad("S01", "Invalid", 7.0); }
    catch (const invalid_argument&) { rejected = true; }
    check(rejected, "Rejecting invalid GPA");

    auto university = makeDemo();
    auto& course = university.findDepartment("CS").findCourse("CS201");
    rejected = false;
    try { course.enroll(university.findStudent("S01")); }
    catch (const invalid_argument&) { rejected = true; }
    check(rejected, "Rejecting duplicate enrollment");

    check(generateReports(university, false) == generateReports(university, true),
          "Single-threaded and multithreaded results match");

    istringstream input("PROFESSOR,P1,Teacher\nDEPARTMENT,CS,Computer Science\n"
                        "ASSIGN_PROF,CS,P1\nSTUDENT,S1,Student,3.5\n"
                        "COURSE,CS,C1,Programming,P1\nENROLL,CS,C1,S1\n");
    auto imported = loadUniversity(input);
    check(imported.findDepartment("CS").findCourse("C1").students.size() == 1,
          "Importing records");

    cout << "5 tests passed.\n";
}

void printReports(const University& university) {
    auto reports = generateReports(university);
    if (reports.empty()) cout << "No courses found.\n";
    for (const auto& report : reports) cout << report;
}

int main(int argc, char* argv[]) {
    try {
        string command = argc > 1 ? argv[1] : "demo";
        if (command == "demo" && argc <= 2) {
            printReports(makeDemo());
        } else if (command == "benchmark" && argc == 2) {
            runBenchmark();
        } else if (command == "test" && argc == 2) {
            runTests();
        } else if (command == "import" && argc == 3) {
            ifstream file(argv[2]);
            if (!file) throw runtime_error("Cannot open file: " + string(argv[2]));
            printReports(loadUniversity(file));
        } else {
            cerr << "Usage: " << argv[0] << " [demo | benchmark | test | import FILE]\n";
            return 2;
        }
        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
