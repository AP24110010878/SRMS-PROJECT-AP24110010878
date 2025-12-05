
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define LINEBUF 256
#define CRED_FILE "credentials.txt"
#define STUD_FILE "students.txt"

// Student structure
struct Student {
    int roll;
    char name[100];
    int age;
    float marks;
};

struct Student s[MAX];
int count = 0;

/* Function declarations */
int loadCredentials(char *user_out, char *pass_out);
int login_prompt(const char *correctUser, const char *correctPass);
void loadStudents();
void saveStudents();
void addStudent();
void displayStudents();
void searchStudent();
void updateStudent();
void deleteStudent();
void clear_stdin();

/* Main */
int main() {
    char correctUser[64] = {0}, correctPass[64] = {0};

    if (!loadCredentials(correctUser, correctPass)) {
        printf("Failed to prepare credentials file. Exiting.\n");
        return 1;
    }

    if (!login_prompt(correctUser, correctPass)) {
        printf("\nAccess Denied! Exiting program...\n");
        return 0;
    }

    printf("\nLogin Successful! Welcome %s.\n", correctUser);

    loadStudents();

    int choice;
    do {
        printf("\n******** STUDENT RECORD MANAGEMENT SYSTEM ********\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student by Roll\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Try again.\n");
            clear_stdin();
            continue;
        }

        switch(choice) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: printf("\nExiting program...\n"); break;
            default: printf("Invalid choice! Try again.\n");
        }

    } while(choice != 6);

    return 0;
}

/* Utility: flush leftover input until newline */
void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
int loadCredentials(char *user_out, char *pass_out) {
    FILE *f = fopen(CRED_FILE, "r");
    if (!f) {
        // create default credentials
        f = fopen(CRED_FILE, "w");
        if (!f) {
            perror("Error creating credentials file");
            return 0;
        }
        fprintf(f, "admin,1234\n");
        fclose(f);
        strcpy(user_out, "admin");
        strcpy(pass_out, "1234");
        printf("credentials.txt not found. Created default credentials: user='admin' pass='1234'\n");
        return 1;
    }

    char line[LINEBUF];
    if (fgets(line, sizeof(line), f) == NULL) {
        fclose(f);
        fprintf(stderr, "credentials.txt is empty. Creating default credentials.\n");
        return loadCredentials(user_out, pass_out); // recreate default
    }
    fclose(f);


    char *comma = strchr(line, ',');
    if (!comma) {
        fprintf(stderr, "Invalid credentials format in %s\n", CRED_FILE);
        return 0;
    }
    *comma = '\0';
    // remove newline from password
    char *pw = comma + 1;
    // strip newline
    pw[strcspn(pw, "\r\n")] = '\0';
    line[strcspn(line, "\r\n")] = '\0';

    strncpy(user_out, line, 63);
    user_out[63] = '\0';
    strncpy(pass_out, pw, 63);
    pass_out[63] = '\0';
    return 1;
}

int login_prompt(const char *correctUser, const char *correctPass) {
    char user[64], pass[64];
    int attempts = 3;
    while (attempts-- > 0) {
        printf("\nEnter Username: ");
        if (scanf("%63s", user) != 1) {
            clear_stdin();
            continue;
        }
        printf("Enter Password: ");
        if (scanf("%63s", pass) != 1) {
            clear_stdin();
            continue;
        }

        if (strcmp(user, correctUser) == 0 && strcmp(pass, correctPass) == 0) {
            return 1;
        }

        printf("Incorrect username/password. Attempts left: %d\n", attempts);
    }
    return 0;
}

void loadStudents() {
    FILE *f = fopen(STUD_FILE, "r");
    if (!f) {
        // Create empty students file
        f = fopen(STUD_FILE, "w");
        if (!f) {
            perror("Error creating students file");
            return;
        }
        fclose(f);
        count = 0;
        printf("students.txt not found. Created empty %s\n", STUD_FILE);
        return;
    }

    char line[LINEBUF];
    count = 0;
    while (fgets(line, sizeof(line), f) != NULL && count < MAX) {

        if (strlen(line) == 0) continue;

        char *token;
        token = strtok(line, "|");
        if (!token) continue;
        s[count].roll = atoi(token);

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(s[count].name, token, sizeof(s[count].name)-1);
        s[count].name[sizeof(s[count].name)-1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        s[count].age = atoi(token);

        token = strtok(NULL, "|");
        if (!token) continue;
        s[count].marks = atof(token);

        count++;
    }
    fclose(f);
}

/* Save current students[] to STUD_FILE */
void saveStudents() {
    FILE *f = fopen(STUD_FILE, "w");
    if (!f) {
        perror("Error opening students file for writing");
        return;
    }
    for (int i = 0; i < count; i++) {
        // Use '|' as delimiter so name can have spaces
        fprintf(f, "%d|%s|%d|%.2f\n", s[i].roll, s[i].name, s[i].age, s[i].marks);
    }
    fclose(f);
}

/* Add student and save */
void addStudent() {
    if (count >= MAX) {
        printf("Cannot add more students. Storage full!\n");
        return;
    }

    int roll;
    char namebuf[100];
    int age;
    float marks;

    printf("\nEnter Roll Number: ");
    if (scanf("%d", &roll) != 1) {
        printf("Invalid roll number.\n");
        clear_stdin();
        return;
    }

    // check duplicate roll
    for (int i = 0; i < count; i++) {
        if (s[i].roll == roll) {
            printf("Roll number %d already exists. Cannot add duplicate.\n", roll);
            return;
        }
    }

    clear_stdin(); // flush before fgets
    printf("Enter Name: ");
    if (fgets(namebuf, sizeof(namebuf), stdin) == NULL) {
        printf("Error reading name.\n");
        return;
    }
    namebuf[strcspn(namebuf, "\r\n")] = '\0'; // trim newline

    printf("Enter Age: ");
    if (scanf("%d", &age) != 1) {
        printf("Invalid age.\n");
        clear_stdin();
        return;
    }

    printf("Enter Marks: ");
    if (scanf("%f", &marks) != 1) {
        printf("Invalid marks.\n");
        clear_stdin();
        return;
    }

    s[count].roll = roll;
    strncpy(s[count].name, namebuf, sizeof(s[count].name)-1);
    s[count].name[sizeof(s[count].name)-1] = '\0';
    s[count].age = age;
    s[count].marks = marks;
    count++;

    saveStudents();
    printf("Student Added Successfully!\n");
}

/* Display all students */
void displayStudents() {
    if (count == 0) {
        printf("\nNo student records available!\n");
        return;
    }

    printf("\n------ All Student Records ------\n");
    for (int i = 0; i < count; i++) {
        printf("\nStudent %d:\n", i + 1);
        printf("Roll Number : %d\n", s[i].roll);
        printf("Name        : %s\n", s[i].name);
        printf("Age         : %d\n", s[i].age);
        printf("Marks       : %.2f\n", s[i].marks);
    }
}

/* Search student by roll */
void searchStudent() {
    int roll, found = 0;
    printf("\nEnter Roll Number to Search: ");
    if (scanf("%d", &roll) != 1) {
        printf("Invalid input.\n");
        clear_stdin();
        return;
    }

    for (int i = 0; i < count; i++) {
        if (s[i].roll == roll) {
            found = 1;
            printf("\nRecord Found!\n");
            printf("Roll Number : %d\n", s[i].roll);
            printf("Name        : %s\n", s[i].name);
            printf("Age         : %d\n", s[i].age);
            printf("Marks       : %.2f\n", s[i].marks);
            break;
        }
    }

    if (!found)
        printf("No student found with roll number %d\n", roll);
}

/* Update student by roll */
void updateStudent() {
    int roll, found = 0;
    printf("\nEnter Roll Number to Update: ");
    if (scanf("%d", &roll) != 1) {
        printf("Invalid input.\n");
        clear_stdin();
        return;
    }

    for (int i = 0; i < count; i++) {
        if (s[i].roll == roll) {
            found = 1;
            clear_stdin();
            char namebuf[100];
            int age;
            float marks;

            printf("\nEnter New Name (press Enter to keep current: %s): ", s[i].name);
            if (fgets(namebuf, sizeof(namebuf), stdin) == NULL) {
                printf("Error reading name.\n");
                return;
            }
            namebuf[strcspn(namebuf, "\r\n")] = '\0';
            if (strlen(namebuf) > 0) {
                strncpy(s[i].name, namebuf, sizeof(s[i].name)-1);
                s[i].name[sizeof(s[i].name)-1] = '\0';
            }

            printf("Enter New Age (current %d, enter 0 to keep): ", s[i].age);
            if (scanf("%d", &age) != 1) {
                printf("Invalid age.\n");
                clear_stdin();
                return;
            }
            if (age > 0) s[i].age = age;

            printf("Enter New Marks (current %.2f, enter negative to keep): ", s[i].marks);
            if (scanf("%f", &marks) != 1) {
                printf("Invalid marks.\n");
                clear_stdin();
                return;
            }
            if (marks >= 0.0f) s[i].marks = marks;

            saveStudents();
            printf("Student Record Updated Successfully!\n");
            break;
        }
    }

    if (!found)
        printf("Student with roll number %d not found!\n", roll);
}

/* Delete student by roll */
void deleteStudent() {
    int roll, found = 0;
    printf("\nEnter Roll Number to Delete: ");
    if (scanf("%d", &roll) != 1) {
        printf("Invalid input.\n");
        clear_stdin();
        return;
    }

    for (int i = 0; i < count; i++) {
        if (s[i].roll == roll) {
            found = 1;
            // Shift left
            for (int j = i; j < count - 1; j++) {
                s[j] = s[j + 1];
            }
            count--;
            saveStudents();
            printf("Student Deleted Successfully!\n");
            break;
        }
    }

    if (!found)
        printf("No student found with roll number %d\n", roll);
}
