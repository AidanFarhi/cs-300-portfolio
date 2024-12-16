#include <iostream>
#include <fstream>
#include <set>

using namespace std;

/**
 * Represents a course with its number, name, and prerequisites.
 */
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};

/**
 * Validates if the line contains at least one comma to separate fields.
 * 
 * @param line The input line from the file.
 * @return True if the line is valid, false otherwise.
 */
bool isValidLine(string line) {
    // count the number of commas in the line
    int numCommas = 0;
    for (int i = 0; i < line.length(); i++) {
        if (line[i] == ',') numCommas++;
    }
    // there must be at least one comma (two fields) for the line to be valid
    if (numCommas < 1) return false;
    return true;
}

/**
 * Extracts the first field from a line until the first comma.
 * 
 * @param line The input line from the file.
 * @return The first field in the line.
 */
string getField(string line) {
    // gets the text of a line up to the first comma found
    return line.substr(0, line.find(','));
}

/**
 * Loads course numbers from a file and inserts them into a set.
 * 
 * @param courseNumbers A set to store unique course numbers.
 * @param fileName The name of the file containing course data.
 */
void loadCourseNumbers(set<string>& courseNumbers, string fileName) {
    // open the file for reading and initialize variables
    ifstream file(fileName);
    string line;
    string courseNumber;
    // iterate through the file and add each course number to the set
    while (getline(file, line)) {
        courseNumber = getField(line);
        courseNumbers.insert(courseNumber);
    }
    file.close();
}

/**
 * Loads courses from a file into an unordered map.
 *
 * @param courses An unordered map to store course data.
 * @param fileName The name of the file containing course data.
 * @return The number of courses loaded.
 */
int loadCourses(unordered_map<string, Course>& courses, string fileName) {
    // load all of the course numbers into a set
    set<string> courseNumbers;
    loadCourseNumbers(courseNumbers, fileName);
    // iterate through the file line-by-line, add each course to the map,
    // and track the number of courses loaded to the map
    string line;
    int numCourses = 0;
    ifstream file(fileName);
    while (getline(file, line)) {
        // if a line is valid, attempt to parse the line and load the course into the map
        if (isValidLine(line)) {
            // this will hold the new course data from the line
            struct Course course;
            // get the first two fields, courseNumber and courseName
            course.courseNumber = getField(line);
            line = line.substr(line.find(',')+1, line.length());
            course.courseName = getField(line);
            line = line.substr(line.find(',')+1, line.length());
            // attempt to add all of the course prerequisites
            while (line.length() > 1 && line != ",") {
                // add the prerequisite to the vector
                if (getField(line).length() > 0) course.prerequisites.push_back(getField(line));
                // if there are no more lines left, break out of the loop
                if (line.find(',') == string::npos) break;
                // move to the next field in the string
                line = line.substr(line.find(',')+1, line.length());
            }
            // validate that each prerequisite is a valid course number
            for (string p : course.prerequisites) {
                // this effectively skips adding the course to the map
                if (courseNumbers.find(p) == courseNumbers.end()) {
                    continue;
                }
            }
            // this only happens if all checks have completed successfully
            courses.insert({course.courseNumber, course});
            numCourses++;
        }
    }
    file.close();
    return numCourses;
}

/**
 * Prints the list of all courses in sorted order by course number.
 * 
 * @param courses An unordered map containing course data.
 */
void printCourses(unordered_map<string, Course> courses) {
    // print out a separate message if there are no courses loaded
    if (courses.size() == 0) {
        cout << "No courses loaded!" << endl;
        return;
    };
    // load all of the course numbers into a vector
    vector<string> courseNumbers;
    for (const auto& pair : courses) {
        courseNumbers.push_back(pair.first);
    }
    // sort the course numbers alphanumerically
    sort(courseNumbers.begin(), courseNumbers.end());
    // print out each course number and name in the sorted vector
    cout << "------- All Courses -------" << endl;
    for (string courseNum : courseNumbers) {
        cout << courseNum << ", " << courses.find(courseNum)->second.courseName << endl;
    }
}

/**
 * Searches for and displays details of a specific course by course number.
 * 
 * @param courses An unordered map containing course data.
 * @param courseNumber The course number to search for.
 */
void findCourse(unordered_map<string, Course> courses, string courseNumber) {
    // check if the course exists in the map
    if (courses.find(courseNumber) != courses.end()) {
        // get the course from the map and print out all of its data
        Course course = courses.at(courseNumber);
        cout << "------- Course Info -------" << endl;
        cout << "Course Number: " << course.courseNumber << endl;
        cout << "Course Name:   " << course.courseName << endl;
        cout << "Prerequisites: ";
        for (int i = 0; i < course.prerequisites.size(); i++) {
            cout << course.prerequisites.at(i);
            if (i != course.prerequisites.size() - 1) cout << ", ";
        }
        cout << endl;
    } else {
        // this only prints if the provided course number was not found in the map
        cout << "Course Number " << courseNumber << " not found" << endl;
    }
}

/**
 * Prints the menu options for the user.
 */
void printMenu() {
    // display all the options
    cout << "--------- Options ---------" << endl;
    cout << "1. Load courses" << endl;
    cout << "2. Print all CS courses" << endl;
    cout << "3. Search for a course" << endl;
    cout << "9. Exit the program" << endl;
    cout << "---------------------------" << endl;
}

/**
 * Handles user input and executes the corresponding functionality.
 * 
 * @param courses An unordered map to store course data.
 * @return True to continue the program, false to exit.
 */
bool handleUserInput(unordered_map<string, Course>& courses) {
    // initalize all of the needed variables
    bool result = true;
    int choice;
    int coursesLoaded = 0;
    string fileName;
    string courseNumber;
    cout << "> ";
    // get input from the user. if an error occurs while reading input,
    // this will gracefully handle that case without an infinite loop
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    // perform an operation based on the user input
    switch (choice) {
        case 1:
            cout << "Enter file name > ";
            cin >> fileName;
            coursesLoaded = loadCourses(courses, fileName);
            cout << coursesLoaded << " courses loaded" << endl;
            break;
        case 2:
            printCourses(courses);
            break;
        case 3:
            cout << "Enter course number > ";
            cin >> courseNumber;
            // this makes sure the search is case-insensitive
            transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);
            findCourse(courses, courseNumber);
            break;
        case 9:
            cout << "Goodbye!" << endl;
            result = false;
            break;
        default:
            cout << "Invalid Option!" << endl;
            break;
    }
    return result;
}

/**
 * Main entry point of the program.
 * 
 * @return Exit status of the program.
 */
int main() {
    // initialize variables
    bool keepRunning = true;
    unordered_map<string, Course> courses;
    // start the main loop
    while (keepRunning) {
        printMenu();
        keepRunning = handleUserInput(courses);
    }
    return 0;
}
