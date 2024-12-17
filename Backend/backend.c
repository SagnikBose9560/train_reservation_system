#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// Global database pointer
sqlite3 *db;

// Function Prototypes
void loginUser(const char *username, const char *password);
void registerUser(const char *username, const char *password);
void viewTrains();
int executeQuery(const char *query, int (*callback)(void *, int, char **, char **), void *data);
int loginCallback(void *data, int argc, char **argv, char **colName);
int trainCallback(void *data, int argc, char **argv, char **colName);

// Main Function
int main() {
    // Open the SQLite database
    if (sqlite3_open("../database/train_reservation.db", &db) != SQLITE_OK) {
        fprintf(stderr, "Error: Cannot open database - %s\n", sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }

    // Open input and output files
    FILE *input = fopen("input.txt", "r");
    if (!input) {
        fprintf(stderr, "Error: Unable to open input file.\n");
        sqlite3_close(db);
        return EXIT_FAILURE;
    }

    FILE *output = fopen("output.txt", "w");
    if (!output) {
        fprintf(stderr, "Error: Unable to open output file.\n");
        fclose(input);
        sqlite3_close(db);
        return EXIT_FAILURE;
    }
    fclose(output); // Close immediately, functions will handle writing

    // Command processing
    char command[50];
    char username[50];
    char password[50];

    if (fscanf(input, "%s", command) != 1) {
        fprintf(stderr, "Error: Failed to read command.\n");
        fclose(input);
        sqlite3_close(db);
        return EXIT_FAILURE;
    }

    if (strcmp(command, "LOGIN") == 0) {
        if (fscanf(input, "%s %s", username, password) == 2) {
            loginUser(username, password);
        } else {
            fprintf(stderr, "Error: Invalid input format for LOGIN.\n");
        }
    } else if (strcmp(command, "REGISTER") == 0) {
        if (fscanf(input, "%s %s", username, password) == 2) {
            registerUser(username, password);
        } else {
            fprintf(stderr, "Error: Invalid input format for REGISTER.\n");
        }
    } else if (strcmp(command, "VIEW_TRAINS") == 0) {
        viewTrains();
    } else {
        fprintf(stderr, "Error: Unknown command '%s'.\n", command);
    }

    fclose(input);
    sqlite3_close(db);
    return EXIT_SUCCESS;
}

// Execute an SQL query with a callback
int executeQuery(const char *query, int (*callback)(void *, int, char **, char **), void *data) {
    char *errMsg = NULL;
    int rc = sqlite3_exec(db, query, callback, data, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    return rc;
}

// Callback for login query
int loginCallback(void *data, int argc, char **argv, char **colName) {
    *(int *)data = atoi(argv[0]); // Store result in the provided memory location
    return 0;
}

// Login user
void loginUser(const char *username, const char *password) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM users WHERE username='%s' AND password='%s';",
             username, password);

    int userExists = 0;
    if (executeQuery(query, loginCallback, &userExists) == SQLITE_OK) {
        FILE *output = fopen("output.txt", "w");
        if (output) {
            if (userExists) {
                fprintf(output, "Login Successful!\n");
            } else {
                fprintf(output, "Invalid Username or Password.\n");
            }
            fclose(output);
        } else {
            fprintf(stderr, "Error: Unable to write to output file.\n");
        }
    } else {
        fprintf(stderr, "Error: Login query execution failed.\n");
    }
}

// Register user
void registerUser(const char *username, const char *password) {
    char query[256];
    snprintf(query, sizeof(query),
             "INSERT INTO users (username, password) VALUES ('%s', '%s');",
             username, password);

    FILE *output = fopen("output.txt", "w");
    if (!output) {
        fprintf(stderr, "Error: Unable to write to output file.\n");
        return;
    }

    char *errMsg = NULL;
    if (sqlite3_exec(db, query, NULL, NULL, &errMsg) != SQLITE_OK) {
        fprintf(output, "Registration Failed: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        fprintf(output, "Registration Successful!\n");
    }
    fclose(output);
}

// Callback for viewing trains
int trainCallback(void *data, int argc, char **argv, char **colName) {
    FILE *output = fopen("output.txt", "a");
    if (!output) {
        fprintf(stderr, "Error: Unable to write to output file.\n");
        return 1;
    }

    for (int i = 0; i < argc; i++) {
        fprintf(output, "%s: %s\n", colName[i], argv[i] ? argv[i] : "NULL");
    }
    fprintf(output, "\n");
    fclose(output);
    return 0;
}

// View train details
void viewTrains() {
    const char *query = "SELECT id, train_name, source, destination, available_seats FROM trains;";
    if (executeQuery(query, trainCallback, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error: Failed to fetch train data.\n");
    }
}

