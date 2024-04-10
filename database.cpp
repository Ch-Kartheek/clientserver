#include <iostream>
#include <sqlite3.h>

#define SQLITE_OK 0

int main() {
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;

    int hallticketno;
    std::string name;
    int maths, physics, chemistry;

    // Input hallticket number from user
    std::cout << "Enter hallticket number: ";
    std::cin >> hallticketno;

    // Open the database
    rc = sqlite3_open("results.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1;
    }

    // Prepare the SQL query with placeholder for hallticketno
    const char* sql = "SELECT name, maths, physics, chemistry FROM student WHERE hallticketno = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1;
    }

    // Bind the hallticket number to the placeholder
    rc = sqlite3_bind_int(stmt, 1, hallticketno);
    if (rc != SQLITE_OK) {
        std::cerr << "Error binding parameter: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return -1;
    }

    // Execute the query and check for results
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Retrieve data from each column
        name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        maths = sqlite3_column_int(stmt, 1);
        physics = sqlite3_column_int(stmt, 2);
        chemistry = sqlite3_column_int(stmt, 3);

        // Use the retrieved data here (e.g., print or store in other variables)
        std::cout << "Name: " << name << std::endl;
        std::cout << "Maths: " << maths << std::endl;
        std::cout << "Physics: " << physics << std::endl;
        std::cout << "Chemistry: " << chemistry << std::endl;
    } else if (rc == SQLITE_DONE) {
        std::cout << "No record found for hallticket number: " << hallticketno << std::endl;
    } else {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
    }

    // Clean up resources
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return 0;
}
