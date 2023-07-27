//-------------------------------includes-------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------------consts and defines---------------------------------

#define LEN 10
#define NUM_OF_GRADES 10
#define LEVELS 12
#define NUM_OF_CLASSES 10

//-----------------------------structs----------------------------------

struct Student {
    char first_name[LEN];
    char last_name[LEN];
    char telephone[LEN];
    int num_layers;
    int num_of_class;
    int grades[NUM_OF_GRADES];
};

struct School {
    struct Student* DB[LEVELS][NUM_OF_CLASSES];
};

//-----------------------------prototypes-------------------------------


void printStudentData(struct Student *student);
int readStudentData(FILE *file, struct Student *student);
int openFileAndProcessStudents(const char *filename, struct Student *students, int *num_students);
void init_db(struct School *school);
void addStudentToDB(struct School *school, struct Student *student);
void displayMenu();
int saveStudentData(const char *filename, struct Student *students, int num_students);
void addNewStudent(struct Student *students, int *num_students, struct School *school);
void viewAllStudents(struct Student *students, int num_students) ;
void processMenuOptions(struct Student *students, int *num_students, struct School *school);
void searchStudent(struct Student *students, int num_students) ;
struct Student* searchStudentByName(struct Student *students, int num_students, const char *first_name, const char *last_name) ;
void editStudentData(struct Student *students, int num_students) ;

//------------------------------main------------------------------------

int main() {
    struct Student students[LEN];
    int num_students = 0;

    struct School school;
    init_db(&school);

    // Check if "db_file.txt" exists
    FILE *db_file = fopen("db_file.txt", "r");
    if (db_file) {
        fclose(db_file);
        // Read student data from "db_file.txt" if it exists
        if (!openFileAndProcessStudents("db_file.txt", students, &num_students)) {
            return EXIT_FAILURE;
        }
    } else {
        // If "db_file.txt" doesn't exist, read from "students.txt" and create the file
        if (!openFileAndProcessStudents("students.txt", students, &num_students)) {
            return EXIT_FAILURE;
        }
        if (!saveStudentData("db_file.txt", students, num_students)) {
            printf("Error saving data to the file.\n");
            return EXIT_FAILURE;
        }
    }

    // Process the menu options
    processMenuOptions(students, &num_students, &school);

    return EXIT_SUCCESS;
}



// Function to handle admission of a new student
void addNewStudent(struct Student *students, int *num_students, struct School *school) {
    if (*num_students >= LEN) {
        printf("Database is full. Cannot add more students.\n");
    } else {
        struct Student newStudent;
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
        students[*num_students] = newStudent;
        (*num_students)++;

        // Save changes to the file after adding the new student
        if (!saveStudentData("db_file.txt", students, *num_students)) {
            printf("Error saving changes to the file.\n");
            exit(EXIT_FAILURE);
        }
    }
}

// Function to handle viewing all students
void viewAllStudents(struct Student *students, int num_students) {
    for (int i = 0; i < num_students; i++) {
        printStudentData(&students[i]);
    }
}

// Function to handle user input and menu options
void processMenuOptions(struct Student *students, int *num_students, struct School *school) {
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
                viewAllStudents(students, *num_students);
                break;
            case 3: // Search for a student
                searchStudent(students, *num_students);
                break;
            case 4: // Edit student data
                editStudentData(students, *num_students);
                break;
            case 5: // Exit
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }

        // ... Existing code ...

    } while (choice != 5);
}





// Function to print student data
void printStudentData(struct Student *student) {
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
int readStudentData(FILE *file, struct Student *student) {
    if (fscanf(file, "%s %s %s %d %d", student->first_name, student->last_name,
               student->telephone, &student->num_layers, &student->num_of_class) != 5) {
        return 0;
    }

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
int openFileAndProcessStudents(const char *filename, struct Student *students, int *num_students) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    while (*num_students < LEN && !feof(file)) {
        if (!readStudentData(file, &students[*num_students])) {
            break;
        }

        (*num_students)++;
    }

    fclose(file);

    return 1;
}

// Function to initialize the school database
void init_db(struct School *school) {
    for (int i = 0; i < LEVELS; i++) {
        for (int j = 0; j < NUM_OF_CLASSES; j++) {
            school->DB[i][j] = NULL;
        }
    }
}

// Function to add a student to the school database
void addStudentToDB(struct School *school, struct Student *student) {
    // Assuming num_layers and num_of_class are valid (within the appropriate range)
    school->DB[student->num_layers - 1][student->num_of_class - 1] = student;
}

// Function to display the menu options
void displayMenu() {
    puts("MENU:");
    puts("1. Add new student");
    puts("2. View all students");
    puts("3. Search for a student");
    puts("4. Edit student data"); // New option for editing student data
    puts("5. Exit");
}


// Function to save student data to the file
int saveStudentData(const char *filename, struct Student *students, int num_students) {
    FILE *file = fopen(filename, "w");
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

// function for search student
struct Student* searchStudentByName(struct Student *students, int num_students, const char *first_name, const char *last_name) {
    for (int i = 0; i < num_students; i++) {
        if (strcmp(students[i].first_name, first_name) == 0 && strcmp(students[i].last_name, last_name) == 0) {
            return &students[i];
        }
    }
    return NULL; // Student not found
}


// function for menu - search student
void searchStudent(struct Student *students, int num_students) {
    char first_name[LEN];
    char last_name[LEN];

    printf("Enter student's first name: ");
    scanf("%s", first_name);
    printf("Enter student's last name: ");
    scanf("%s", last_name);

    struct Student *foundStudent = searchStudentByName(students, num_students, first_name, last_name);
    if (foundStudent != NULL) {
		puts("\nResult:\n-------------------");
        printStudentData(foundStudent);
    } else {
        printf("Student not found.\n");
    }
}


// Function to edit student data
void editStudentData(struct Student *students, int num_students) {
    char first_name[LEN];
    char last_name[LEN];

    printf("Enter student's first name: ");
    scanf("%s", first_name);
    printf("Enter student's last name: ");
    scanf("%s", last_name);

    struct Student *foundStudent = searchStudentByName(students, num_students, first_name, last_name);
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
        if (!saveStudentData("db_file.txt", students, num_students)) {
            printf("Error saving changes to the file.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Student not found.\n");
    }
}



