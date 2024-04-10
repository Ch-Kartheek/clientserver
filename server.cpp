#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include <vector>
#include <thread>
#include <tuple>

// Function to handle each client connection
void handleClient(int clientSocket, int clientNumber, std::map<int, int>& clientMap, std::map<std::string, std::tuple<int, int, int>>& studentData) {
    while (true) {
        char idBuffer[256];
        ssize_t bytesReceived = recv(clientSocket, idBuffer, sizeof(idBuffer), 0);
        if (bytesReceived <= 0) {
            // If no data received or an error occurred, check if client is still connected
            if (bytesReceived == 0)
                std::cout << "Client " << clientNumber << " disconnected." << std::endl;
            else
                std::cerr << "Error receiving data from client " << clientNumber << std::endl;

            close(clientSocket);
            clientMap.erase(clientNumber);
            return;
        }

        std::cout << "Received ID number from Client " << clientNumber << ": " << idBuffer << std::endl;

        std::string idString(idBuffer);
        auto it = studentData.find(idString);
        if (it != studentData.end()) {
            int mathMarks, physicsMarks, chemistryMarks;
            std::tie(mathMarks, physicsMarks, chemistryMarks) = it->second;
            send(clientSocket, &mathMarks, sizeof(mathMarks), 0);
            send(clientSocket, &physicsMarks, sizeof(physicsMarks), 0);
            send(clientSocket, &chemistryMarks, sizeof(chemistryMarks), 0);
            std::cout << "Sent marks to Client " << clientNumber << std::endl;
        } else {
            const char* notFoundMessage = "Data not found.";
            send(clientSocket, notFoundMessage, strlen(notFoundMessage), 0);
            std::cout << "Data not found for Client " << clientNumber << std::endl;
        }
    }
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(5789);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Bind failed." << std::endl;
        close(serverSocket);
        return 1;
    }

    listen(serverSocket, 5);

    std::cout << "Server is listening..." << std::endl;

    std::map<std::string, std::tuple<int, int, int>> studentData = {
        {"1", std::make_tuple(90, 85, 78)},
        {"2", std::make_tuple(85, 80, 75)},
        {"3", std::make_tuple(92, 88, 80)},
        {"4", std::make_tuple(88, 82, 79)},
        {"5", std::make_tuple(89, 84, 77)}
    };

    std::map<int, int> clientMap;
    int clientNumber = 1;

    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddrLen = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection." << std::endl;
            continue;
        }

        clientMap[clientNumber] = clientSocket;
        std::thread(handleClient, clientSocket, clientNumber, std::ref(clientMap), std::ref(studentData)).detach();
        std::cout << "Client " << clientNumber << " connected." << std::endl;
        clientNumber++;
    }

    close(serverSocket);

    return 0;
}
