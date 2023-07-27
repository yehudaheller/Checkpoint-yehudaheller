//-------------------------------includes-------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------------consts and defines---------------------------------

#define LEN 1000
#define NUM_OF_GRADES 10
#define LEVELS 12
#define NUM_OF_CLASSES 10
#define FAIL_GRADE 50

//-----------------------------structs----------------------------------

struct Student {
    char* first_name;
    char* last_name;
    char* telephone;
    int num_layers;
    int num_of_class;
    int grades[NUM_OF_GRADES];
};

struct School {
    struct Student* DB[LEVELS][NUM_OF_CLASSES];
};

//-----------------------------prototypes-------------------------------

void printStudentData(struct Student* student);
int readStudentData(FILE* file, struct Student* student);
int openFileAndProcessStudents(const char* filename, struct Student** students, int* num_students);
void init_db(struct School* school);
void addStudentToDB(struct School* school, struct Student* student);
void displayMenu();
int saveStudentData(const char* filename, struct Student* students, int num_students);
void addNewStudent(struct Student** students, int* num_students, struct School* school);
void viewAllStudents(struct Student* students, int num_students);
void processMenuOptions(struct Student** students, int* num_students, struct School* school);
void searchStudent(struct Student* students, int num_students);
struct Student* searchStudentByName(struct Student* students, int num_students, const char* first_name, const char* last_name);
void editStudentData(struct Student** students, int num_students);
void deleteStudent(struct Student** students, int* num_students, struct School* school);
void freeStudentMemory(struct Student* student);
void findCandidatesForDeparture(struct Student* students, int num_students, int grade_threshold);
double calculateAverage(struct Student* students, int num_students, int grade_num, int num_layers) ;
void calculateAverageForGradeAndLayer(struct Student* students, int num_students) ;
//------------------------------main------------------------------------

// Declare gradeNumberToCompare as a global variable
int gradeNumberToCompare;

int main() {
    struct Student* students = (struct Student*)malloc(LEN * sizeof(struct Student));
    int num_students = 0;

    struct School school;
    init_db(&school);

    // Check if "db_file.txt" exists
    FILE* db_file = fopen("db_file.txt", "r");
    if (db_file) {
        fclose(db_file);
        // Read student data from "db_file.txt" if it exists
        if (!openFileAndProcessStudents("db_file.txt", &students, &num_students)) {
            return EXIT_FAILURE;
        }
    } else {
        // If "db_file.txt" doesn't exist, read from "students.txt" and create the file
        if (!openFileAndProcessStudents("students.txt", &students, &num_students)) {
            return EXIT_FAILURE;
        }
        if (!saveStudentData("db_file.txt", students, num_students)) {
            printf("Error saving data to the file.\n");
            return EXIT_FAILURE;
        }
    }

    // Process the menu options
    processMenuOptions(&students, &num_students, &school);

    // Free allocated memory before exiting
    for (int i = 0; i < num_students; i++) {
        freeStudentMemory(&students[i]);
    }
    free(students);

    return EXIT_SUCCESS;
}

// Function to handle admission of a new student
void addNewStudent(struct Student** students, int* num_students, struct School* school) {
    if (*num_students >= LEN) {
        printf("Database is full. Cannot add more students.\n");
    } else {
        struct Student newStudent;
        newStudent.first_name = (char*)malloc(LEN);
        newStudent.last_name = (char*)malloc(LEN);
        newStudent.telephone = (char*)malloc(LEN);

        printf("Enter student's first name: ");
        scanf("%s", newStudent.first_name);
        printf("Enter student's last name: ");
        scanf("%s", newStudent.last_name);
        printf("Enter student's telephone: ");
        scanf("%s", newStudent.telephone);
        printf("Enter student's number of layers: ");
        scanf("%d", &newStudent.num_layers);
        printf("Enter student's number of class: ");
        scanf("%d", &newStudent.num_of_class);
        printf("Enter student's grades (%d grades): ", NUM_OF_GRADES);
        for (int i = 0; i < NUM_OF_GRADES; i++) {
            scanf("%d", &newStudent.grades[i]);
        }

        // Add the new student to the database
        addStudentToDB(school, &newStudent);
        (*students)[*num_students] = newStudent;
        (*num_students)++;

        // Save changes to the file after adding the new student
        if (!saveStudentData("db_file.txt", *students, *num_students)) {
            printf("Error saving changes to the file.\n");
            exit(EXIT_FAILURE);
        }
    }
}

// Function to handle viewing all students
void viewAllStudents(struct Student* students, int num_students) {
    for (int i = 0; i < num_students; i++) {
        printStudentData(&students[i]);
    }
}

// Function to print student data
void printStudentData(struct Student* student) {
    printf("Name: %s %s\n", student->first_name, student->last_name);
    printf("Telephone: %s\n", student->telephone);
    printf("Number of Layers: %d\n", student->num_layers);
    printf("Number of class: %d\n", student->num_of_class);

    printf("Grades:\n");
    for (int i = 0; i < NUM_OF_GRADES; i++) {
        printf("%d ", student->grades[i]);
    }
    printf("\n\n");
}

// Function to read student data from the file
int readStudentData(FILE* file, struct Student* student) {
    char first_name[LEN];
    char last_name[LEN];
    char telephone[LEN];

    if (fscanf(file, "%s %s %s %d %d", first_name, last_name,
               telephone, &student->num_layers, &student->num_of_class) != 5) {
        return 0;
    }

    // Allocate memory for the strings and copy the data
    student->first_name = strdup(first_name);
    student->last_name = strdup(last_name);
    student->telephone = strdup(telephone);

    // Read grades until the end of the line
    for (int i = 0; i < NUM_OF_GRADES; i++) {
        int grade;
        if (fscanf(file, "%d", &grade) != 1) {
            // If reading a grade fails, break out of the loop
            break;
        }
        student->grades[i] = grade;
    }

    return 1;
}

// Function to open file and process students
int openFileAndProcessStudents(const char* filename, struct Student** students, int* num_students) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    while (*num_students < LEN && !feof(file)) {
        if (!readStudentData(file, &(*students)[*num_students])) {
            break;
        }

        (*num_students)++;
    }

    fclose(file);

    return 1;
}

// Function to initialize the school database
void init_db(struct School* school) {
    for (int i = 0; i < LEVELS; i++) {
        for (int j = 0; j < NUM_OF_CLASSES; j++) {
            school->DB[i][j] = NULL;
        }
    }
}

// Function to add a student to the school database
void addStudentToDB(struct School* school, struct Student* student) {
    // Assuming num_layers and num_of_class are valid (within the appropriate range)
    school->DB[student->num_layers - 1][student->num_of_class - 1] = student;
}

// Function to edit student data
void editStudentData(struct Student** students, int num_students) {
    char first_name[LEN];
    char last_name[LEN];

    printf("Enter student's first name: ");
    scanf("%s", first_name);
    printf("Enter student's last name: ");
    scanf("%s", last_name);

    struct Student* foundStudent = searchStudentByName(*students, num_students, first_name, last_name);
    if (foundStudent != NULL) {
        // Allow editing the fields
        printf("Enter student's telephone: ");
        scanf("%s", foundStudent->telephone);
        printf("Enter student's number of layers: ");
        scanf("%d", &foundStudent->num_layers);
        printf("Enter student's number of class: ");
        scanf("%d", &foundStudent->num_of_class);
        printf("Enter student's grades (%d grades): ", NUM_OF_GRADES);
        for (int i = 0; i < NUM_OF_GRADES; i++) {
            scanf("%d", &foundStudent->grades[i]);
        }

        // Save changes to the file after editing
        if (!saveStudentData("db_file.txt", *students, num_students)) {
            printf("Error saving changes to the file.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Student not found.\n");
    }
}

// Function to search for a student by name
struct Student* searchStudentByName(struct Student* students, int num_students, const char* first_name, const char* last_name) {
    for (int i = 0; i < num_students; i++) {
        if (strcmp(students[i].first_name, first_name) == 0 && strcmp(students[i].last_name, last_name) == 0) {
            return &students[i];
        }
    }
    return NULL; // Student not found
}

// Function to search for a student
void searchStudent(struct Student* students, int num_students) {
    char first_name[LEN];
    char last_name[LEN];

    printf("Enter student's first name: ");
    scanf("%s", first_name);
    printf("Enter student's last name: ");
    scanf("%s", last_name);

    struct Student* foundStudent = searchStudentByName(students, num_students, first_name, last_name);
    if (foundStudent != NULL) {
        puts("\nResult:\n-------------------");
        printStudentData(foundStudent);
    } else {
        printf("Student not found.\n");
    }
}

// Function to delete existing student
void deleteStudent(struct Student** students, int* num_students, struct School* school) {
    char first_name[LEN];
    char last_name[LEN];

    printf("Enter student's first name: ");
    scanf("%s", first_name);
    printf("Enter student's last name: ");
    scanf("%s", last_name);

    // Find the student in the array by name
    struct Student* foundStudent = searchStudentByName(*students, *num_students, first_name, last_name);
    if (foundStudent != NULL) {
        // Remove the student from the school database
        school->DB[foundStudent->num_layers - 1][foundStudent->num_of_class - 1] = NULL;

        // Find the index of the student in the array
        int index = -1;
        for (int i = 0; i < *num_students; i++) {
            if (strcmp((*students)[i].first_name, first_name) == 0 && strcmp((*students)[i].last_name, last_name) == 0) {
                index = i;
                break;
            }
        }

        // Shift the elements in the array to overwrite the deleted student
        if (index >= 0) {
            freeStudentMemory(&(*students)[index]);

            for (int i = index; i < *num_students - 1; i++) {
                (*students)[i] = (*students)[i + 1];
            }
            (*num_students)--;

            // Save changes to the file after deletion
            if (!saveStudentData("db_file.txt", *students, *num_students)) {
                printf("Error saving changes to the file.\n");
                exit(EXIT_FAILURE);
            }

            printf("Student '%s %s' has been deleted.\n", first_name, last_name);
        } else {
            printf("Student not found.\n");
        }
    } else {
        printf("Student not found.\n");
    }
}

// Function to free the memory allocated for a student's fields
void freeStudentMemory(struct Student* student) {
    free(student->first_name);
    free(student->last_name);
    free(student->telephone);
}

// Function to save student data to the file
int saveStudentData(const char* filename, struct Student* students, int num_students) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    for (int i = 0; i < num_students; i++) {
        fprintf(file, "%s %s %s %d %d", students[i].first_name, students[i].last_name,
                students[i].telephone, students[i].num_layers, students[i].num_of_class);

        for (int j = 0; j < NUM_OF_GRADES; j++) {
            fprintf(file, " %d", students[i].grades[j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
}

// Function for option 7
void findCandidatesForDeparture(struct Student* students, int num_students, int grade_threshold) {
    printf("Candidates for Departure:\n");
    for (int i = 0; i < num_students; i++) {
        int failing_grades = 0;
        for (int j = 0; j < NUM_OF_GRADES; j++) {
            if (students[i].grades[j] < grade_threshold) {
                failing_grades++;
            }
        }
        // Modify the criteria here according to your requirements.
        // For example, you could check for failing grades in multiple subjects or other conditions.

        if (failing_grades >= 3) {
            printStudentData(&students[i]);
        }
    }
}




// for 8:
// Function to prompt the user for the grade and num_layers and calculate the average
void calculateAverageForGradeAndLayer(struct Student* students, int num_students) {
    int grade_num, num_layers;
    printf("Enter the desired course number (1-10): ");
    scanf("%d", &grade_num);
    printf("Enter the desired layer (1-12): ");
    scanf("%d", &num_layers);
    
    double average = calculateAverage(students, num_students, grade_num, num_layers);
    if (average >= 0) {
        printf("\n Average grade for students in course %d and layer %d \n is : %.2f\n \n", grade_num, num_layers, average);
    } else {
        printf("No students found with grade %d and num_layers = %d\n", grade_num, num_layers);
    }
}

// Function to calculate the average of students with a specific grade and num_layers
double calculateAverage(struct Student* students, int num_students, int grade_num, int num_layers) {
    int totalGrade = 0;
    int count = 0;

    for (int i = 0; i < num_students; i++) {
        if (students[i].num_layers == num_layers ) {
            totalGrade += students[i].grades[grade_num - 1];
            count++;
        }
    }

    if (count > 0) {
        return (double)totalGrade / count;
    } else {
        return -1.0; // Return a negative value to indicate no students found
    }
}
// Function for displaying the main menu
void displayMenu() {
    puts("MENU:");
    puts("1. Add new student");
    puts("2. View all students");
    puts("3. Search for a student");
    puts("4. Edit student data");
    puts("5. Delete a student");
    puts("6. Find top 10 students with the specified grade in every level");
    puts("7. Find candidates for departure");
    puts("8. Calculate average for a specific courese and layer");
    puts("9. Exit");
}

// Function for processing the menu options
void processMenuOptions(struct Student** students, int* num_students, struct School* school) {
    int choice;
    do {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: // Admission of a new student
                addNewStudent(students, num_students, school);
                break;
            case 2: // View all students
                viewAllStudents(*students, *num_students);
                break;
            case 3: // Search for a student
                searchStudent(*students, *num_students);
                break;
            case 4: // Edit student data
                editStudentData(students, *num_students);
                break;
            case 5: // Delete a student
                deleteStudent(students, num_students, school);
                break;
            case 6: // Find top 10 students with the specified grade in every level
                // ...
                break;
            case 7: // Find candidates for departure
                findCandidatesForDeparture(*students, *num_students, FAIL_GRADE); // Modify the grade threshold as needed
                break;
            case 8: // Calculate average for a specific grade and num_layers
                calculateAverageForGradeAndLayer(*students, *num_students);
                break;
            case 9: // Exit
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 9);
}


