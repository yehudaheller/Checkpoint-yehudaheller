#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 10
#define NUM_OF_GRADES 10
#define LEVELS 12
#define NUM_OF_CLASSES 10
#define NUM_OF_Courses 20

struct Student {
    char first_name[LEN];
    char last_name[LEN];
    char telephone[LEN];
    int num_layers;
    int num_of_class; // New member to store the number of classes
    int grades[NUM_OF_GRADES]; // Use NUM_OF_GRADES instead of LEN for grades array
};


//struct School {
//	struct Student* DB[LEVELS][NUM_OF_CLASSES]
//}

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

int main() {
    struct Student students[LEN];
    int num_students = 0;

    if (!openFileAndProcessStudents("students.txt", students, &num_students)) {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < num_students; i++) {
        printStudentData(&students[i]);
    }

    return EXIT_SUCCESS;
}
