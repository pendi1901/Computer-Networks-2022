#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fstream>
#include <pthread.h>
#define PORT 8888
#define HOST "0.0.0.0"
using namespace std;
// globally initialise the file

void *process(void *p_clientSocket){
    FILE *file;
    file = fopen("ThreadOutput.txt", "a");

    int clientSocket = *((int *)p_clientSocket);



    // While loop: accept and echo message back to client
    char buffer[4096];
    while (true)
    {
        // Clear the buffer
        memset(buffer, 0, 4096);

        // Wait for client to send data
        int result = recv(clientSocket, buffer, 4096, 0);
        if (result == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }

        if (result == 0)
        {
            cout << "Client disconnected " << endl;
           
            break;
        }

        cout << "Received: " << string(buffer, 0, result) << endl;
        fprintf(file, "Received: %s", string(buffer, 0, result).c_str());

        // Find factorial of the number
        int num = atoi(buffer);
        //  unsigned long
        long fact = 1;
        for ( int i = 1; i <= num; i++){
            fact *= i;
        }
        fprintf(stderr, "Factorial Sent: %ld\n", fact);

        // Send the factorial to the client
        send(clientSocket, to_string(fact).c_str(), to_string(fact).size() + 1, 0);
    }

    // log_file.close();
    fclose(stderr);
    close(clientSocket);

    return NULL;
}

int main()
{   
    FILE* file;
    file = fopen( "ThreadOutput.txt", "a");
    
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
    // cout << "Server the thread way intialised" << endl;
    while (true){
        cout << "Waiting for connection" << endl;

        // int client_sock = accept_conn(serversocket, filename);
    

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
            fprintf(file, "Client Id: %s connected on %d", host, ntohs(client.sin_port));
        }


        pthread_t thread_id;
        int *p_client = &clientSocket;

        pthread_create(&thread_id, NULL, process, p_client);
        cout << "Creating thread " << &thread_id << endl;
        fclose(file);
    }
    return 0;
}