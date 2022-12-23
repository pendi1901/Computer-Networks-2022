#include "node.h"
#include <queue>
#include <climits>
#include <unordered_map>
#include <iostream>
#include <vector>

using namespace std;

void printRT(vector<Node *> network){
  //Printing them routing table thingis here
  for (int i = 0; i < network.size(); i++){

    network[i]->printTable();
  }
}

// unordered_map<string, vector<RoutingEntry>> network_sys;
// vector<pair<string, routingtbl>> edges;

void routingAlgo(vector<Node *> network){
  // Initialize the array
  int n = network.size();
  int network_graph[n][n];

  // Fill the 2d array with zeroes
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j < n; ++j)
    {
      network_graph[i][j] = 0;
    }
  }

  //convert this for each loop into a for loop
  Node *router = network[0];
  for(int i = 0; i < network.size(); i++){
    network[i]->start_flood("start");
    network[i]->sendMsg();
  }
  

  for (Node *node : network)
  {
    node->recvMsg(network, node, "Node");
  }

  printf("Printing the routing tables after the convergence \n");
  printRT(network);
}

//This function is used to find the minimum distance between the nodes
//This function is used in the Dijkstra's algorithm


int minDistance(std:: vector<int> mylist, int moved[], int n){
  int low = INT_MAX;
  int low_id;

  for (int i = 0; i < n; i++)
    if(moved[i] != 0){
      continue;
    } 
    else{
      if(mylist[i]<=low){
        low = mylist[i];
        low_id = i;
      }
    }

  return low_id;
}

//Function to find the IP Address of the sender 

string ip_Address_Sender(Node *p, Node *d){
  vector<pair<NetInterface, Node *>> interfaces = p->getinterfaces();
  string sender=d->getName();
  for (int i = 0; i < interfaces.size(); ++i){
    if(interfaces[i].second->getName() != sender){
      continue;
    }
    else{
      return interfaces[i].first.getip();
    }
  }
  return "Failed to get the ip address of the sender";
}

//Function to find the IP Address of the receiver 

string ip_Address_Receiver(Node *p, Node *d){
  vector<pair<NetInterface, Node *>> interfaces = p->getinterfaces();
  string receiver= d->getName();
  for (int i = 0; i < interfaces.size(); ++i){
    if (interfaces[i].second->getName() !=receiver){
      continue;
    }
    else{
      return interfaces[i].first.getConnectedIp();
    }
  }
  return "Failed to get the ip address of the receiver";
}

// Function to find the cost of the link between the sender and the receiver
// Cost is the weight of the link

int getTheNoOfHops(vector<Node *> nd, int i, int parent[]){
  while (!(parent[parent[i]] == -1))
    i = parent[i];
  return i;
}




void Node::recvMsg(vector<Node *> network, Node *sender, string stt){
  int id = 0;
  int len = network.size();
  int n = len;
  int distanceArray[len];
  int  travelled[len];
  int god[len];
  int  **whole_network_sys = new int *[n];
  for (int i = 0; i < n; ++i){
    whole_network_sys[i] = new int[n];
    distanceArray[i] = INT_MAX;
    travelled[i] = 0;
    god[i] = -1;
  }
  for (int i = 0; i < len; ++i){
    for (auto n : network[i]->neighbours){
      int n_id = n.first.getid();
      int net_id = whole_network_sys[i][n_id];
      whole_network_sys[i][n_id] = n.second;

    }
  }
  int senderid= sender->getid();
  distanceArray[senderid] = id;
  int length = len -1;

  for (int count = 0; count < length; count++){
    vector<int> nums;
    for (int i=0; i < len; i++) {
        nums.push_back(distanceArray[i]);
    }
    int minDist_id = minDistance(nums, travelled, len);
    travelled[minDist_id] = 1;
    int loopiter = network[minDist_id]->neighbours.size();
    for (int i = 0; i < loopiter; i++){
      Node* a = network[minDist_id];
      int node_id = a->neighbours[i].first.getid();
      // int ed = network[minDist_id]->neighbours[i].first.getid();
      if (!travelled[node_id] && whole_network_sys[minDist_id][node_id]){
        int first  = distanceArray[minDist_id] + whole_network_sys[minDist_id][node_id];
        int second = distanceArray[node_id];
        
        bool bl = first < second;
        if (!(distanceArray[minDist_id] == INT_MAX) && bl){
          distanceArray[node_id] = distanceArray[minDist_id] + whole_network_sys[minDist_id][node_id];
          god[node_id] = minDist_id;
        }
      }
    }
  }

  if (stt == "update"){
    for (int i = 0; i < n; i++){
      if(god[i] == -1){
        continue;
      }
      else{
        int total_hops = getTheNoOfHops(network, i, god);
        for(int j=0 ; j< network[i]->ips.size(); j++){
          sender->addTblEntry(network[i]->ips[j], ip_Address_Receiver(sender, network[total_hops]), ip_Address_Sender(sender, network[total_hops]), distanceArray[i]);
        }
      }
    }
    return;
  }

  for (int i = 0; i < n; i++){
    if(god[i] == -1){
      continue;
    }
    else{
      int total_hops = getTheNoOfHops(network, i, god);
      for (int j = 0; j < network[i]->ips.size(); j++){
        sender->addTblEntry(network[i]->ips[j], ip_Address_Receiver(sender, network[total_hops]), ip_Address_Sender(sender, network[total_hops]), distanceArray[i]);
      }
    }
  }
}
