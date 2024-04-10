#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // Connect to the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP address
    serverAddress.sin_port = htons(5789);                    // Server port

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to server." << std::endl;
        close(clientSocket);
        return 1;
    }

    while (true) {
        // Prompt user for ID number
        std::cout << "Enter your ID number (or type 'quit' to exit): ";
        std::string idNumber;
        std::cin >> idNumber;

        if (idNumber == "quit") {
            break;
        }

        // Send ID number to server
        send(clientSocket, idNumber.c_str(), idNumber.size(), 0);

        // Receive and display subject marks from server
        int mathMarks, physicsMarks, chemistryMarks;
        recv(clientSocket, &mathMarks, sizeof(mathMarks), 0);
        recv(clientSocket, &physicsMarks, sizeof(physicsMarks), 0);
        recv(clientSocket, &chemistryMarks, sizeof(chemistryMarks), 0);

        std::cout << "Maths Marks: " << mathMarks << std::endl;
        std::cout << "Physics Marks: " << physicsMarks << std::endl;
        std::cout << "Chemistry Marks: " << chemistryMarks << std::endl;
    }

    // Close socket
    close(clientSocket);

    return 0;
}
