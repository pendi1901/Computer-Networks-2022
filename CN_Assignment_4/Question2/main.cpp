#include "node.h"

vector<Node *> distanceVectorNodes;

void routingAlgo(vector<Node *> distanceVectorNodes);
// void routingAlgo2(vector<Node*> distanceVectorNodes);

int main()
{
  int n; // number of nodes
  cin >> n;
  string name; // Node label
  distanceVectorNodes.clear();
  for (int i = 0; i < n; i++)
  {
    Node *newnode = new Node();
    cin >> name;
    newnode->setName(name);
    newnode->setid(distanceVectorNodes.size());
    distanceVectorNodes.push_back(newnode);
  }
  cin >> name;
  /*
    For each node label(@name), it's own ip address, ip address of another node
    defined by @oname will be inserted in the node's own datastructure interfaces
  */
  int cost;
  while (name != "EOE")
  { // End of entries
    for (int i = 0; i < distanceVectorNodes.size(); i++)
    {
      string myeth, oeth, oname;
      if (distanceVectorNodes[i]->getName() == name)
      {
        // node interface ip
        cin >> myeth;
        // ip of another node connected to myeth (nd[i])
        cin >> oeth;
        // label of the node whose ip is oeth
        cin >> oname;
        cin >> cost;
        for (int j = 0; j < distanceVectorNodes.size(); j++)
        {
          if (distanceVectorNodes[j]->getName() == oname)
          {
            /*
            @myeth: ip address of my (distanceVectorNodes[i]) end of connection.
            @oeth: ip address of other end of connection.
            @distanceVectorNodes[j]: pointer to the node whose one of the interface is @oeth
            */
            distanceVectorNodes[i]->addInterface(myeth, oeth, distanceVectorNodes[j], cost);
            // Routing table initialization
            /*
            @myeth: ip address of my (distanceVectorNodes[i]) ethernet interface.
            @0: hop count, 0 as node does not need any other hop to pass packet to itself.

            */
            distanceVectorNodes[i]->addTblEntry(myeth, 0);
            break;
          }
        }
      }
    }
    cin >> name;
  }

  /* The logic of the routing algorithm should go here */
  routingAlgo(distanceVectorNodes);
  /* Add the logic for periodic update (after every 1 sec) here */
  // routingAlgo(distanceVectorNodes);
}
