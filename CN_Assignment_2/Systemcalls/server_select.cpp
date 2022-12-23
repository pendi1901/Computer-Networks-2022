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

using namespace std;

int init_server(int port)
{
    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        cerr << "Error creating socket" << endl;
        return -1;
    }

    // Bind the socket to an Ip address and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    // Bind the socket

    bind(sock, (sockaddr *)&hint, sizeof(hint));

    // Mark the socket for listening in
    if (listen(sock, SOMAXCONN) == -1)
    {
        cerr << "Error listening" << endl;
        return -1;
    }

    return sock;
}

int accept_conn(int server_sock, string filename)
{

    fstream log_file;
    log_file.open(filename, ios::app);

    if (!log_file)
    {
        cout << "File not created!";
        return -1;
    }

    // Accept a call
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];    // Client's remote name
    char service[NI_MAXSERV]; // Service (i.e. port) the client is connect on

    int clientSocket = accept(server_sock, (sockaddr *)&client, &clientSize);

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
        log_file << "Client Id: " << host << " connected on " << ntohs(client.sin_port) << endl;
    }

    log_file.close();
    return clientSocket;
}

int handle_conn(int clientSocket, string filename)
{

    fstream log_file;
    log_file.open(filename, ios::app);

    if (!log_file)
    {
        cout << "File not created!";
        return -1;
    }

    // While loop: accept and echo message back to client
    char buf[4096];
    while (true)
    {
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
            log_file << "Client disconnected " << endl;
            break;
        }

        cout << "Received: " << string(buf, 0, bytesReceived) << endl;
        log_file << "Received: " << string(buf, 0, bytesReceived) << endl;

        // Find factorial of the number
        int num = atoi(buf);
        //  unsigned long
        unsigned long long int fact = 1;
        for (unsigned long long int i = 1; i <= num; i++)
        {
            fact *= i;
        }

        log_file << "Factorial Sent: " << fact << endl;

        // Send the factorial to the client
        send(clientSocket, to_string(fact).c_str(), to_string(fact).size() + 1, 0);
    }

    log_file.close();
    close(clientSocket);

    return 0;
};

int main()
{

    int server_sock = init_server(54000);

    if (server_sock == -1)
    {
        return -1;
    }

    fd_set current_sockets, ready_sockets;
    FD_ZERO(&current_sockets);
    FD_SET(server_sock, &current_sockets);

    string fileName = "selectserver_log.txt";

    cout << "Server(select()) intialised..." << endl;

    while (true)
    {
        cout << "Waiting for connection" << endl;

        ready_sockets = current_sockets;

        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
        {
            cout << "Error in select" << endl;
            return -1;
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &ready_sockets))
            {
                if (i == server_sock)
                {
                    int client_sock = accept_conn(server_sock, fileName);
                    if (client_sock == -1)
                    {
                        return -1;
                    }
                    FD_SET(client_sock, &current_sockets);
                }
                else
                {
                    handle_conn(i, fileName);
                    FD_CLR(i, &current_sockets);
                }
            }
        }
    }

    return 0;
}