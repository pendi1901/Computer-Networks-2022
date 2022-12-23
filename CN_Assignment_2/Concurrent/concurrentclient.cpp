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

using namespace std;

void *connection(void *args){
 
    // Create a socket
    int clientsock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsock == -1)
    {
        cerr << "Error creating socket" << endl;
        return NULL;
    }

    // Connect to a remote socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(8888);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    int connectRes = connect(clientsock, (sockaddr *)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        cerr << "Error connecting to server" << endl;
        return NULL;
    }

    // Do-while loop to send and receive data
    char buffer[4096];
    string userInput;

    int i = 1;
    do
    {
        // Prompt the user for some text
        cout << "Client> " << i << endl;

        // Send the text
        int sendRes = send(clientsock, to_string(i).c_str(), to_string(i).size() + 1, 0);
        if (sendRes == -1)
        {
            cerr << "Error sending to server" << endl;
            continue;
        }

        // Wait for response
        memset(buffer, 0, 4096);
        int bytesReceived = recv(clientsock, buffer, 4096, 0);
        if (bytesReceived == -1)
        {
            cerr << "Error receiving from server" << endl;
        }
        else
        {
            // Display response
            cout << "Server Response> " << string(buffer, 0, bytesReceived) << endl;
        }

        i++;
    } while (i < 21);

    // Close the socket
    close(clientsock);

    return NULL;
}

int main()
{
    int numberOfClients = 10;

    pthread_t threads[numberOfClients];

    for (int i = 0; i < numberOfClients; i++)
    {
        pthread_create(&threads[i], NULL, connection, NULL);
        cout << "Creating thread " << &threads[i] << endl;
    }

    for (int i = 0; i < numberOfClients; i++)
    {
        cout << "Thread " << &threads[i] << " joining" << endl;
        pthread_join(threads[i], NULL);
    }

    return 0;
}