#include <iostream>
#include "SQLite/sqlite3.h"
#include <string>

// Callback function to handle query results
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}

// Function to initialize database
int initializeDatabase(sqlite3* db) {
    char* errMsg = 0;
    // Create table for employees
    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS EMPLOYEES(" \
        "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
        "NAME TEXT NOT NULL," \
        "SALARY REAL NOT NULL);";

    int rc = sqlite3_exec(db, createTableSQL, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return 1;
    }
    return 0;
}

// Function to add an employee
int addEmployee(sqlite3* db, const std::string& name, double salary) {
    char* errMsg = 0;
    std::string addEmployeeSQL = "INSERT INTO EMPLOYEES (NAME, SALARY) VALUES ('" + name + "', " + std::to_string(salary) + ");";

    int rc = sqlite3_exec(db, addEmployeeSQL.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return 1;
    }
    std::cout << "Employee added successfully" << std::endl;
    return 0;
}

// Function to calculate payroll
void calculatePayroll(sqlite3* db) {
    const char* selectEmployeesSQL = "SELECT * FROM EMPLOYEES;";
    char* errMsg = 0;

    int rc = sqlite3_exec(db, selectEmployeesSQL, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

int main() {
    sqlite3* db;
    char* errMsg = 0;

    // Open database
    int rc = sqlite3_open("payroll.db", &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }
    else {
        std::cout << "Opened database successfully" << std::endl;
    }

    // Initialize database
    if (initializeDatabase(db) != 0) {
        std::cerr << "Database initialization failed" << std::endl;
        sqlite3_close(db);
        return 1;
    }

    // Menu
    int choice = 0;
    while (choice != 4) {
        std::cout << "1. Add Employee\n";
        std::cout << "2. Calculate Payroll\n";
        std::cout << "3. View Employees\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            std::string name;
            double salary;
            std::cout << "Enter employee name: ";
            std::cin.ignore();
            std::getline(std::cin, name);
            std::cout << "Enter employee salary: ";
            std::cin >> salary;
            if (addEmployee(db, name, salary) != 0) {
                std::cerr << "Failed to add employee" << std::endl;
            }
            break;
        }
        case 2: {
            std::cout << "Calculating Payroll...\n";
            calculatePayroll(db);
            break;
        }
        case 3: {
            std::cout << "Employees:\n";
            const char* selectEmployeesSQL = "SELECT * FROM EMPLOYEES;";
            char* errMsg = 0;

            int rc = sqlite3_exec(db, selectEmployeesSQL, callback, 0, &errMsg);
            if (rc != SQLITE_OK) {
                std::cerr << "SQL error: " << errMsg << std::endl;
                sqlite3_free(errMsg);
            }
            break;
        }
        case 4: {
            std::cout << "Exiting...\n";
            break;
        }
        default:
            std::cout << "Invalid choice\n";
        }
    }

    // Close database
    sqlite3_close(db);
    return 0;
}
