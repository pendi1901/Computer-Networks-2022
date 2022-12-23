#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
using namespace std;
// #define PORT 8888;

int main(){

    // Creating a socket
    int clientsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsocket == -1)
    {
        cerr << "Error: Unsuccesful socket creation" << endl;
        return -1;
    }

    // Connecting to remote socket
    int PORT = 8888;
    char* host= "0.0.0.0";
    sockaddr_in sockaddr1;
    sockaddr1.sin_family = AF_INET;
    sockaddr1.sin_port = htons(PORT);
    inet_pton(AF_INET, host, &sockaddr1.sin_addr);
    int connector = connect(clientsocket, (sockaddr*)&sockaddr1, sizeof(sockaddr1));
    if (connector == -1)
    {
        cerr << "Error: Unsucessful connection to Server" << endl;
        return -1;
    }

    // For loop to send and receive data to the server
    char buffer[4096];
    string input;
    int i = 1;
    for (int i  = 1;  i < 21 ; i++){
        // Ask for input
        cout << "Client Input:  " << i << endl;

        // Sending the input to the server
        int response = send(clientsocket, to_string(i).c_str(), to_string(i).size() + 1, 0);
        if (response == -1)
        {
            cerr << "Error: Unsuccesful Sending Input to server" << endl;
            continue;
        }

        // Clear the buffer pre waiting for response from server
        memset(buffer, 0, 4096);
        // Await response from server
        int responseserver = recv(clientsocket, buffer, 4096, 0);
        if (responseserver == -1)
        {
            cerr << "Error: Unsuccessful response receive from Server" << endl;
        }
        else
        {
            // Display response
            cout << "Server Response:  " << string(buffer, 0, responseserver) << endl;
        }
    } 

    // Close the socket
    close(clientsocket);

    return 0;
}