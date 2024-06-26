#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"
#include <ctype.h>
//5
USER user;
static char filename[MAX_NAME_LENGTH + 5];
static PASS* array_of_passwords = NULL;
static PASS* found_password = NULL;
int number_of_pass = 0;

static enum Choice2 choice2;

void createfile(const char* const filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

//10 11
void new_user(int* const num_users, USER* users, const char* const file_users) {
    if (*num_users >= MAX_USERS) {
        printf("User capacity limit reached, please remove a user or expand memory.\n");
    }
    else {
        char new_username[MAX_NAME_LENGTH];
        int i;
        do {
            printf("Enter your name: ");
            scanf(" %49[^\n]", new_username);
            while (getchar() != '\n'); // Clear input buffer

            if (strchr(new_username, ' ') != NULL) {
                printf("Spaces are not allowed in usernames! Please try again.\n");
            }
            else if (!is_alpha_string(new_username)) {
                printf("Only letters are allowed , please try again.\n");
            }
            else {
                break;
            }
        } while (1);

        for (i = 0; i < *num_users; i++) {
            if (strcmp(new_username, users[i].name) == 0) {
                printf("Username '%s' already exists. Please choose a different username.\n", new_username);
                return;
            }
        }

        users[*num_users].name = (char*)malloc(MAX_NAME_LENGTH * sizeof(char));
        if (users[*num_users].name == NULL) {
            printf("Memory allocation failed for user name.\n");
            return;
        }
        memset(users[*num_users].name, 0, MAX_NAME_LENGTH * sizeof(char));

        users[*num_users].password = (char*)malloc(MAX_PASS_LENGTH * sizeof(char));
        if (users[*num_users].password == NULL) {
            printf("Memory allocation failed for user password.\n");
            free(users[*num_users].name);
            return;
        }
        memset(users[*num_users].password, 0, MAX_PASS_LENGTH * sizeof(char));

        strcpy(users[*num_users].name, new_username);

        do {
            printf("Enter your password: ");
            scanf(" %19[^\n]", users[*num_users].password);
            while (getchar() != '\n');

            if (strchr(users[*num_users].password, ' ') != NULL) {
                printf("Spaces are not allowed in passwords! Please try again.\n");
            }
            else if (!validatePlaintext(users[*num_users].password)) {
                printf("Error: Password contains disallowed characters. \n");
            }
            else {
                break;
            }
        } while (1);

        users[*num_users].num_passwords = 0;

        printf("User %s successfully created.\n", users[*num_users].name);

        FILE* file = fopen(file_users, "a");
        if (file == NULL) {
            perror("Error opening file");
            free(users[*num_users].name);
            free(users[*num_users].password);
            return;
        }
        encryptXOR(users[*num_users].password);

        fprintf(file, "%s %s\n", users[*num_users].name, users[*num_users].password);
        fclose(file);

        (*num_users)++;
    }
}

USER* load_users(const char* file_users, int* num_users) {
    FILE* file = fopen(file_users, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    USER* users = malloc(sizeof(USER) * MAX_USERS);
    if (users == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    *num_users = 0;
    char name[MAX_NAME_LENGTH];
    char password[MAX_PASS_LENGTH];
    while (fscanf(file, "%49s %19s", name, password) == 2) {
        //  decryptXOR(password); (dissabled because it messes up log in decryption)
        users[*num_users].name = _strdup(name);
        users[*num_users].password = _strdup(password);
        users[*num_users].num_passwords = 0;
        (*num_users)++;
    }

    fclose(file);
    return users;
}


void login_user(int* const num_users, USER* users) {
    int i = 0;
    //for (i = 0; i < *num_users; i++) {  //(its quick check up to see what kidn of passwords u recieve)
    //    printf("User %s password: %s \n", users[i].name, users[i].password);
    //}
    char input2[100];

    char username[MAX_NAME_LENGTH];
    char password[MAX_PASS_LENGTH];

    do {
        printf("Enter your username: ");
        scanf(" %49[^\n]", username);
        while (getchar() != '\n');

        if (strchr(username, ' ') != NULL) {
            printf("Spaces are not allowed in usernames! Please try again.\n");
        }
        else {
            break;
        }
    } while (1);



    do {
        printf("Enter your password: ");
        scanf(" %19[^\n]", password);
        while (getchar() != '\n');

        if (strchr(password, ' ') != NULL) {
            printf("Spaces are not allowed in passwords! Please try again.\n");
        }
        else {
            break;
        }
    } while (1);
    char filename[MAX_NAME_LENGTH + 4]; // +4 for .txt

    for (i = 0; i < *num_users; i++) {
        // printf("User %s password: %s \n", users[i].name, users[i].password); //quick check
        decryptXOR(users[i].password);
        // printf("User %s password: %s \n", users[i].name, users[i].password); //quick check
        if (strcmp(username, users[i].name) == 0 && strcmp(password, users[i].password) == 0) {
            printf("Login successful.\n");
            encryptXOR(users[i].password);
            system("pause");
            snprintf(filename, sizeof(filename), "%s.txt", username);


            if (_access(filename, 0) == 0) {

                printf("File %s already exists.\n", filename);
            }
            else {

                createfile(filename);
                printf("File %s created successfully.\n", filename);
            }
            break;
        }
    }
    if (i == *num_users) {
        printf("Invalid username or password.\n");
        return;
    }


    while (1) {
        system("cls");

        printf("Unesite izbor:\n1. Enter password you want to save and its usage.\n2. Write out passwords.\n3. Delete password.\n4. Change password by name of use \n5. Search name of use.\n6. Write out passwords name of use alphabetically. \n7. Log out.\n");
        //used for protection from spaces
        if (fgets(input2, sizeof(input2), stdin)) {
            char* endptr;
            long choice2 = strtol(input2, &endptr, 10);


            if (endptr != input2) {
                while (isspace((unsigned char)*endptr)) {
                    endptr++;
                }
                if (*endptr == '\0' || *endptr == '\n') {

                    while (*endptr != '\n' && *endptr != '\0') {
                        endptr++;
                    }
                    switch (choice2) {
                    case ADD_PASSWORD:
                        add_password(filename, &users[i]);
                        if (array_of_passwords != NULL) {
                            free(array_of_passwords);
                            array_of_passwords = NULL;
                        }
                        array_of_passwords = update_password(filename, &number_of_pass);
                        if (array_of_passwords == NULL) {
                            exit(EXIT_FAILURE);
                        }
                        break;

                    case WRITEOUT_PASSWORDS: writeout_passwords(filename, &users[i]);  break;

                    case DELETE_PASSWORD: delete_password(filename, &users[i]); break;

                    case CHANGE_PASSWORD: change_password(filename, &users[i]); break;

                    case SEARCH_PASSWORD: search_password(filename); break;

                    case ABC_PRINT: abc_print(filename, &users[i]); break;

                    case LOG_OUT:
                        printf("You have logged out!\n"); return; break;

                    default:  printf("Invalid choice. Please enter a number between 1 and 6.\n");  break;
                    }
                    system("pause");
                }
                else {
                    // Invalid input
                    printf("Invalid input. Please enter a valid integer.\n");
                    system("pause");
                }
            }
        }
    }
    system("pause");
}



void delete_user(int* const num_users, USER* users) {
    char answer;
    char username[MAX_NAME_LENGTH];
    int i = 0;
    int found = 0;

    printf("Enter username to delete: ");
    scanf("%49s", username);

    printf("Do you really want to delete user: %s? (Y/n)\n", username);
    scanf(" %c", &answer);


    if (answer == 'Y' || answer == 'y') {
        for (i = 0; i < *num_users; i++) {
            if (strcmp(username, users[i].name) == 0) {
                found = 1;
                (*num_users)--;

                char filename[MAX_FILENAME_LENGTH];
                snprintf(filename, MAX_FILENAME_LENGTH, "%s.txt", username);
                if (remove(filename) == 0) {
                    printf("Text file for user '%s' deleted.\n", username);
                }
                else {
                    printf("Unable to delete text file for user '%s'.\n", username);
                }

                for (; i < *num_users; i++) {
                    strcpy(users[i].name, users[i + 1].name);
                    strcpy(users[i].password, users[i + 1].password);
                }
                printf("User '%s' deleted.\n", username);
                break;
            }
        }

        // Updating users_all.txt file because of deleted user
        if (found) {
            FILE* file = fopen("users_all.txt", "r");
            if (file == NULL) {
                perror("Failed to open users_all.txt");
                return;
            }

            char line[MAX_NAME_LENGTH + MAX_NAME_LENGTH + 2];
            char temp_filename[MAX_FILENAME_LENGTH];
            snprintf(temp_filename, MAX_FILENAME_LENGTH, "temp_users_all.txt");
            FILE* temp_file = fopen(temp_filename, "w");
            if (temp_file == NULL) {
                perror("Failed to open temporary file");
                fclose(file);
                return;
            }

            while (fgets(line, sizeof(line), file)) {
                char file_username[MAX_NAME_LENGTH];
                sscanf(line, "%s", file_username);
                if (strcmp(file_username, username) != 0) {
                    fputs(line, temp_file);
                }
            }

            fclose(file);
            fclose(temp_file);

            if (remove("users_all.txt") != 0) {
                perror("Failed to delete users_all.txt");
            }
            else if (rename(temp_filename, "users_all.txt") != 0) {
                perror("Failed to rename temporary file");
            }
        }
    }
    else if (answer == 'N' || answer == 'n') {
        printf("Username isn't deleted.\n");
        return;
    }

    if (found == 0) {
        printf("User '%s' not found.\n", username);
    }
}

void display_users(int* const num_users, USER* users) {
    if (*num_users == 0) {
        printf("No users found.\n");
        return;
    }
    printf("Users:\n");
    for (int i = 0; i < *num_users; i++) {
        printf("%s\n", users[i].name);
    }
}
//16
void free_users(int* const num_users, USER* users) {
    for (int i = 0; i < *num_users; i++) {
        free(users[i].name);
        free(users[i].password);
    }
    *num_users = 0;
}

void exit_manager(int* const num_users, USER* users) {
    char answer = 0;

    printf("Do you really want to exit the password manager?\n");
    printf("If you choose Y or y, you will exit program\n");
    printf("If you do not want to exit, choose N or n.\n");


    scanf(" %c", &answer);

    if (answer == 'Y' || answer == 'y') {

        free_users(num_users, users);
        *num_users = 0;
        exit(EXIT_SUCCESS);
    }
    if (answer == 'N' || answer == 'n') {
        return;
    }
    else {
        printf("Invalid choice. Please choose Y/y or N/n.\n");
    }
}

void create_users_file(const char* const file_users) {
    FILE* file = fopen(file_users, "w");
    if (file == NULL) {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}


//previous idea for project , didnt work too well
//void delete_user_all(int* const num_users, USER* users) {
//
//    int i = 0;
//
//    for (i = 0; i < *num_users; i++) {
//        char filename[MAX_FILENAME_LENGTH];
//        snprintf(filename, MAX_FILENAME_LENGTH, "%s.txt", users[i].name);
//        if (remove(filename) == 0) {
//            printf("Text file for user '%s' deleted.\n", users[i].name);
//        }
//        else {
//            printf("Unable to delete text file for user '%s'.\n", users[i].name);
//        }
//
//    }
//}

//8
inline bool is_alpha_string(const char* str) {
    while (*str) {
        if (!((*str >= 'A' && *str <= 'Z') || (*str >= 'a' && *str <= 'z'))) {
            return false;
        }
        str++;
    }
    return true;
}
