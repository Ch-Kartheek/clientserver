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
    serverAddress.sin_port = htons(6080);                    // Server port

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to server." << std::endl;
        close(clientSocket);
        return 1;
    }

    while (true) {
        // Prompt user for hall ticket number
        std::cout << "Enter your hall ticket number (or type 'quit' to exit): ";
        std::string hallTicketNo;
        std::cin >> hallTicketNo;

        if (hallTicketNo == "quit") {
            break;
        }

        // Send hall ticket number to server
        send(clientSocket, hallTicketNo.c_str(), hallTicketNo.size(), 0);

        // Receive and display subject marks from server
        int maths, physics, chemistry;

        // Receive marks
        recv(clientSocket, &maths, sizeof(maths), 0);
        recv(clientSocket, &physics, sizeof(physics), 0);
        recv(clientSocket, &chemistry, sizeof(chemistry), 0);

        std::cout << "Maths Marks: " << maths << std::endl;
        std::cout << "Physics Marks: " << physics << std::endl;
        std::cout << "Chemistry Marks: " << chemistry << std::endl;
    }

    // Close socket
    close(clientSocket);

    return 0;
}
