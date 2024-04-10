#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#include <thread>
#include <vector>

#define SQLITE_OK 0

// Function to handle each client connection
void handleClient(int clientSocket, sqlite3* db) {
    while (true) {
        char idBuffer[256];
        ssize_t bytesReceived = recv(clientSocket, idBuffer, sizeof(idBuffer), 0);
        if (bytesReceived <= 0) {
            if (bytesReceived == 0)
                std::cout << "Client disconnected." << std::endl;
            else
                std::cerr << "Error receiving data from client." << std::endl;

            close(clientSocket);
            return;
        }

        std::string idString(idBuffer);

        // Prepare the SQL query with placeholder for hallticketno
        const char* sql = "SELECT maths, physics, chemistry FROM student WHERE hallticketno = ?";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
            close(clientSocket);
            return;
        }

        // Bind the hallticket number to the placeholder
        int hallticketno = std::stoi(idString);
        rc = sqlite3_bind_int(stmt, 1, hallticketno);
        if (rc != SQLITE_OK) {
            std::cerr << "Error binding parameter: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            close(clientSocket);
            return;
        }

        // Execute the query and send results to client
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int maths = sqlite3_column_int(stmt, 0);
            int physics = sqlite3_column_int(stmt, 1);
            int chemistry = sqlite3_column_int(stmt, 2);
            // Send results to client
            send(clientSocket, &maths, sizeof(maths), 0);
            send(clientSocket, &physics, sizeof(physics), 0);
            send(clientSocket, &chemistry, sizeof(chemistry), 0);
            std::cout << "Sent marks to client." << std::endl;
        } else {
            const char* notFoundMessage = "Data not found.";
            send(clientSocket, notFoundMessage, strlen(notFoundMessage), 0);
            std::cout << "Data not found for client." << std::endl;
        }

        // Clean up resources
        sqlite3_finalize(stmt);
    }
}

int main() {
    sqlite3* db;
    int rc = sqlite3_open("results.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        sqlite3_close(db);
        return 1;
    }

    // Bind to address and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(5789);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Bind failed." << std::endl;
        close(serverSocket);
        sqlite3_close(db);
        return 1;
    }

    // Listen for incoming connections
    listen(serverSocket, 5);
    std::cout << "Server is listening..." << std::endl;

    // Vector to store thread objects
    std::vector<std::thread> threads;

    while (true) {
        // Accept incoming connection
        sockaddr_in clientAddress;
        socklen_t clientAddrLen = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection." << std::endl;
            continue;
        }

        // Create a new thread to handle the client
        threads.emplace_back(handleClient, clientSocket, db);
    }

    // Close server socket and database connection (should never reach here)
    close(serverSocket);
    sqlite3_close(db);

    return 0;
}
