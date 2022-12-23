#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fstream>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
#define PORT 8888
#define HOST "0.0.0.0"

void *process(void *p_clientSocket){
    int clientSocket = *((int *)p_clientSocket);
    FILE *file;
    file = fopen("ForkOutput.txt", "a");
    // While loop: accept and echo message back to client
    char buf[4096];
    while (true){
        // Clear the buffer
        memset(buf, 0, 4096);

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }

        if (bytesReceived == 0)
        {
            cout << "Client disconnected " << endl;
            // write to a file
            fprintf(file, "Client disconnected\n");
            break;
        }

        cout << "Received: " << string(buf, 0, bytesReceived) << endl;
        fprintf(file, "Received: %s\n", string(buf, 0, bytesReceived).c_str());

        // Find factorial of the number
        int num = atoi(buf);
        long fact = 1;
        for ( int i = 1; i <= num; i++){
            fact *= i;
        }
        fprintf(file, "Factorial Sent: %ld\n", fact);

        // Send the factorial to the client
        send(clientSocket, to_string(fact).c_str(), to_string(fact).size() + 1, 0);
    }

    fclose(file);
    close(clientSocket);

    return NULL;
}

int main()
{

    int serversocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serversocket == -1)
    {
        cerr << "Error creating socket" << endl;
        return -1;
    }

    // Bind the socket to an Ip address and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST, &hint.sin_addr);

    // Bind the socket

    bind(serversocket, (sockaddr *)&hint, sizeof(hint));

    // Mark the socket for listening in
    if (listen(serversocket, SOMAXCONN) == -1)
    {
        cerr << "Error listening" << endl;
        return -1;
    }

    if (serversocket == -1)
    {
        return -1;
    }

    FILE *filename;
    filename = fopen("ForkOutput.txt", "a");

    cout << "Server(thread()) intialised..." << endl;

    while (true)
    {
        cout << "Waiting for connection" << endl;

        // Accept a call
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        char host[NI_MAXHOST];    // Client's remote name
        char service[NI_MAXSERV]; // Service (i.e. port) the client is connect on

        int clientSocket = accept(serversocket, (sockaddr *)&client, &clientSize);

        if (clientSocket == -1)
        {
            cerr << "Error accepting client" << endl;
            return -1;
        }

        // Display information about the client
        int result = getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0);

        if (result)
        {
            cout << host << " connected on " << service << endl;
        }
        else
        {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST); // Convert from binary to string
            cout << host << " connected on " << ntohs(client.sin_port) << endl;
            fprintf(filename, "Client Id: %s connected on %d\n", host, ntohs(client.sin_port));
        }

        fclose(filename);

        // handle_conn(client_sock, fileName);

        // threading using fork
        int id = fork();
        if (id == 0)
        {
            process(&clientSocket);
            exit(0);
        }
    }

    return 0;
}