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

void *process(void *args)
{
    // Client

    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        cerr << "Error creating socket" << endl;
        return NULL;
    }

    // Connect to a remote socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(8888);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    int connectRes = connect(sock, (sockaddr *)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        cerr << "Error connecting to server" << endl;
        return NULL;
    }

    // Do-while loop to send and receive data
    char buf[4096];
    string userInput;

    int i = 1;
    do
    {
        // Prompt the user for some text
        cout << "Client> " << i << endl;

        // Send the text
        int sendRes = send(sock, to_string(i).c_str(), to_string(i).size() + 1, 0);
        if (sendRes == -1)
        {
            cerr << "Error sending to server" << endl;
            continue;
        }

        // Wait for response
        memset(buf, 0, 4096);
        int bytesReceived = recv(sock, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cerr << "Error receiving from server" << endl;
        }
        else
        {
            // Display response
            cout << "Server Response> " << string(buf, 0, bytesReceived) << endl;
        }

        i++;
    } while (i < 21);

    // Close the socket
    close(sock);

    return NULL;
}

int main()
{
    int clients = 10;

    pthread_t threads[clients];

    for (int i = 0; i < clients; i++)
    {
        pthread_create(&threads[i], NULL, process, NULL);
        cout << "Creating thread " << &threads[i] << endl;
    }

    for (int i = 0; i < clients; i++)
    {
        cout << "Thread " << &threads[i] << " joining" << endl;
        pthread_join(threads[i], NULL);
    }

    return 0;
}