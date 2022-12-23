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
#include <sys/poll.h>

using namespace std;

int init_server(int port)
{
    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        cout << "Error creating socket" << endl;
        return -1;
    }

    // Bind the socket to an Ip address and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    // Bind the socket to the IP and port
    bind(sock, (sockaddr *)&hint, sizeof(hint));

    // Mark the socket for listening in
    if (listen(sock, SOMAXCONN) == -1)
    {
        cout << "Error listening" << endl;
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
        cout << "Error accepting client" << endl;
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

    // While loop: accept and echo f message back to client
    char buf[4096];
    while (true)
    {
        // Clear the buffer
        memset(buf, 0, 4096);

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cout << "Error in recv(). Quitting" << endl;
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

    cout << "Server(poll()) intialised..." << endl;

    // Create an array of pollfd structures
    pollfd fds[100];
    int nfds = 1;
    int timeout = 1000;

    // Add the server socket to the array
    fds[0].fd = server_sock;
    fds[0].events = POLLIN;

    while (true)
    {
        cout << "Waiting for connection" << endl;

        // Wait for an event to happen
        int ret = poll(fds, nfds, timeout);

        if (ret == -1)
        {
            cout << "poll error" << endl;
            break;
        }

        if (ret == 0)
        {
            printf("%d seconds elapsed.", timeout / 1000);
            continue;
        }

        for (int i = 0; i < nfds; ++i)
        {
            if (fds[i].revents == 0)
                continue;

            if (fds[i].revents != POLLIN)
            {
                printf("Error! revents = %d", fds[i].revents);
                break;
            }

            if (fds[i].fd == server_sock)
            {
                int client_sock = accept_conn(server_sock, "servpoll_log.txt");
                fds[nfds].fd = client_sock;
                fds[nfds].events = POLLIN;
                ++nfds;
            }
            else
            {
                handle_conn(fds[i].fd, "servpoll_log.txt");
                close(fds[i].fd);
                fds[i].fd = -1;
                --nfds;
                for (int j = i; j < nfds; ++j)
                    fds[j] = fds[j + 1];
                --i;
            }

            if (nfds == 100)
            {
                printf("Too many connections");
                break;
            }

            if (nfds == 1 || nfds == 0)
            {
                printf("No more connections");
                break;
            }
        }
    }

    return 0;
}