#include "SelectionGraph.h"
#include <sstream>
#include <cassert>
#include <iostream>
#include "GroupRepository.h"

using namespace std;


/***********************
  * SelectionGraphNode *
  **********************/

int SelectionGraphNode::nextNodeID = 0;

SelectionGraphNode::SelectionGraphNode():
    id(nextNodeID++),
    selection(NULL),
    selected(false)
{}

SelectionGraphNode::SelectionGraphNode(Selection* selection,Group group):
    id(nextNodeID++),
    selection(selection),
    group(group),
    selected(false)
{}

SelectionGraphNode::SelectionGraphNode(SelectionGraphNode *v):
    id(nextNodeID++),
    group(v->getGroup()),
    selected(v->isSelected()){
    Selection* sel = v->getSelection();
    if(sel == NULL)
        selection = NULL;
    else
        selection = new Selection(sel->getGeometry(),sel->getType());
}

SelectionGraphNode::~SelectionGraphNode(){
    //    if(selection != NULL)
    //        delete selection;

    int numInEdges = inDegree();
    for(int i = numInEdges - 1 ; i >= 0 ; --i){
        SelectionGraphEdge* edge = getInEdge(i);
        SelectionGraphNode* ops = edge->opposite(this);
        ops->removeOutEdge(edge->getID());

        delete edge;
    }

    int numOutEdges = outDegree();
    for(int i = numOutEdges - 1 ; i >= 0 ; --i){
        SelectionGraphEdge* edge = getOutEdge(i);
        SelectionGraphNode* ops = edge->opposite(this);
        ops->removeInEdge(edge->getID());

        delete edge;
    }
}

int SelectionGraphNode::getId(){
    return id;
}

int SelectionGraphNode::inDegree(){
    return inEdges.size();
}

SelectionGraphEdge* SelectionGraphNode::getInEdge(int index){
    return inEdges.at(index);
}

int SelectionGraphNode::outDegree(){
    return outEdges.size();
}

SelectionGraphEdge* SelectionGraphNode::getOutEdge(int index){
    return outEdges.at(index);
}

std::string SelectionGraphNode::str(){
    stringstream ss;
    ss << "v" << id << " I: ";

    int numInDegree = inDegree();
    for(int i = 0 ; i < numInDegree ; ++i){
        SelectionGraphEdge* edge = inEdges.at(i);
        ss << edge->std() << "  ";
    }


    ss << " O: ";
    int numOutDegree = outDegree();
    for(int i = 0 ; i < numOutDegree ; ++i){
        SelectionGraphEdge* edge = outEdges.at(i);
        ss << edge->std() << "  ";
    }

    return ss.str();
}

void SelectionGraphNode::addInEdge(SelectionGraphEdge* e){
    inEdges.push_back(e);
}

void SelectionGraphNode::addOutEdge(SelectionGraphEdge* e){
    outEdges.push_back(e);
}

void SelectionGraphNode::removeInEdge(int edgeID){

    int numInEdges = inDegree();
    for(int i = 0 ; i < numInEdges ; ++i){
        SelectionGraphEdge* edge = inEdges.at(i);
        if(edgeID == edge->getID()){
            inEdges.erase(inEdges.begin() + i);
            break;
        }
    }

}

void SelectionGraphNode::removeOutEdge(int edgeID){
    int numOutEdges = outDegree();
    for(int i = 0 ; i < numOutEdges ; ++i){
        SelectionGraphEdge* edge = outEdges.at(i);
        if(edgeID == edge->getID()){
            outEdges.erase(outEdges.begin() + i);
            break;
        }
    }

}

Selection* SelectionGraphNode::getSelection(){
    return selection;
}

Group SelectionGraphNode::getGroup()
{
    return group;
}


void SelectionGraphNode::setGroup(const Group &g){
    group = g;
}

void SelectionGraphNode::copyGroupToNeighbors(){
    list<SelectionGraphEdge*> toHandle;
    toHandle.assign(inEdges.begin(),inEdges.end());
    toHandle.insert(toHandle.end(),outEdges.begin(),outEdges.end());

    while(!toHandle.empty()){
        SelectionGraphEdge* edge = toHandle.front();
        toHandle.pop_front();
        SelectionGraphNode* neigh = edge->opposite(this);
        if(!neigh->getGroup().equals(this->group)){
            neigh->setGroup(this->group);
            edge->setGroup(this->group);
            //since the graphs are not supposed to be large this should be fine
            neigh->copyGroupToNeighbors();
        }
    }
}

void SelectionGraphNode::setSelected(bool v){
    selected = v;
}

void SelectionGraphNode::toogleSelected(){
    selected = !selected;
}

bool SelectionGraphNode::isSelected(){
    return selected;
}

/***********************
  * SelectionGraphEdge *
  **********************/

int SelectionGraphEdge::nextEdgeID = 0;

SelectionGraphEdge::SelectionGraphEdge(SelectionGraphNode* tail,SelectionGraphNode* head,Group group):
    id(nextEdgeID++),
    tail(tail),
    head(head),
    group(group),
    selected(false)
{
    tail->addOutEdge(this);
    head->addInEdge(this);
}

SelectionGraphEdge::~SelectionGraphEdge(){
    assert(tail != NULL && head != NULL);

    tail->removeOutEdge(id);
    head->removeInEdge(id);
}

SelectionGraphNode* SelectionGraphEdge::getTail(){
    return tail;
}

SelectionGraphNode* SelectionGraphEdge::getHead(){
    return head;
}

SelectionGraphNode* SelectionGraphEdge::opposite(SelectionGraphNode* v){
    assert(v!=NULL);
    int vID = v->getId();

    if(vID == tail->getId()){
        return head;
    }
    else if(vID == head->getId()){
        return tail;
    }
    else{
        cout << "Opposite error" << endl;
        exit(1);
    }
}

int SelectionGraphEdge::getID(){
    return id;
}

std::string SelectionGraphEdge::std(){
    stringstream ss;
    ss << "e(" << tail->getId() << " , " << head->getId() << ")";

    return ss.str();
}

Group SelectionGraphEdge::getGroup(){
    return group;
}

void SelectionGraphEdge::setGroup(Group group)
{
    this->group = group;
}

void SelectionGraphEdge::setSelected(bool v){
    selected = v;
}

void SelectionGraphEdge::toogleSelected(){
    selected = !selected;
}

bool SelectionGraphEdge::isSelected(){
    return selected;
}

/*******************
  * SelectionGraph *
  ******************/

SelectionGraph::SelectionGraph(){

}

SelectionGraph::~SelectionGraph(){
    //cout << "TODO Selectoin Graph Destructor" << endl;
}

SelectionGraphNode* SelectionGraph::addNode(){
    cerr << "Should not be used, SelectionGraph::addNode()" << endl;
    exit(1);
}

SelectionGraphNode* SelectionGraph::addNode(Selection *s){
    SelectionGraphNode* node = new SelectionGraphNode(s, GroupRepository::getInstance().getDefaultItem());
    nodes[node->getId()] = node;
    return node;
}

void SelectionGraph::addNode(SelectionGraphNode *node){
    assert(nodes.count(node->getId()) == 0);
    nodes[node->getId()] = node;
}

SelectionGraphEdge* SelectionGraph::addEdge(){
    cerr << "Should not be used, SelectionGraph::addNode()" << endl;
    exit(1);
}

SelectionGraphEdge* SelectionGraph::addEdge(SelectionGraphNode* tail,SelectionGraphNode* head){
    SelectionGraphEdge* edge = new SelectionGraphEdge(tail,head, GroupRepository::getInstance().getDefaultItem());//does not ensure that the graph is simple
    edges[edge->getID()] = edge;

    return edge;
}

void SelectionGraph::assign(SelectionGraph *g){
    //cout << "Graph Assign" << endl;
    assert(g != NULL);

    //
    SelectionGraph::NodeIterator begin;
    SelectionGraph::NodeIterator end;
    while(numberOfNodes() > 0){
        getNodeIterator(begin,end);
        SelectionGraphNode* node = begin->second;
        removeNode(node->getId());
    }

    if(!(numberOfNodes() == 0 && numberOfEdges() == 0)){
        cout << "Num Nodes " << numberOfNodes() << " Num Edges " << numberOfEdges() << endl;
        assert(numberOfNodes() == 0 && numberOfEdges() == 0);
    }

    g->getNodeIterator(begin,end);
    map<int,SelectionGraphNode*> mapPreviousIdsToNewNodes;
    for(;begin != end; ++begin){
        SelectionGraphNode* node = begin->second;
        SelectionGraphNode* newNode = new SelectionGraphNode(node);
        mapPreviousIdsToNewNodes[node->getId()] = newNode;
        addNode(newNode);
    }

    EdgeIterator edgeBegin,edgeEnd;
    g->getEdgeIterator(edgeBegin,edgeEnd);
    for(; edgeBegin != edgeEnd ; ++edgeBegin){
        SelectionGraphEdge* edge = edgeBegin->second;
        SelectionGraphNode* tail = mapPreviousIdsToNewNodes[edge->getTail()->getId()];
        SelectionGraphNode* head = mapPreviousIdsToNewNodes[edge->getHead()->getId()];
        SelectionGraphEdge* newEdge = addEdge(tail,head);
        newEdge->setGroup(edge->getGroup());
    }
}

SelectionGraphNode* SelectionGraph::getNodeByID(int nodeID){
    assert(nodes.count(nodeID)>0);
    return nodes[nodeID];
}

void SelectionGraph::getNodeIterator(SelectionGraph::NodeIterator& nodesBegin,
                                     SelectionGraph::NodeIterator& nodesEnd){
    nodesBegin = nodes.begin();
    nodesEnd = nodes.end();
}

void SelectionGraph::getNodeReverseIterator(SelectionGraph::NodeReverseIterator& rbegin,
                                            SelectionGraph::NodeReverseIterator& rend){
    rbegin = nodes.rbegin();
    rend   = nodes.rend();
}

void SelectionGraph::getEdgeIterator(SelectionGraph::EdgeIterator &beginIterator,
                                     SelectionGraph::EdgeIterator &endIterator){
    beginIterator = edges.begin();
    endIterator   = edges.end();
}

void SelectionGraph::removeNode(int nodeID){
    if(nodes.count(nodeID)==0)
        return;

    map<int,SelectionGraphNode*>::iterator it = nodes.find(nodeID);
    SelectionGraphNode* node = it->second;

    //deletes from the graph
    nodes.erase(it);
    int numInEdges = node->inDegree();
    for(int i = numInEdges - 1 ; i >= 0 ; --i){
        SelectionGraphEdge* edge = node->getInEdge(i);
        edges.erase(edge->getID());
    }

    int numOutEdges = node->outDegree();
    for(int i = numOutEdges - 1 ; i >= 0 ; --i){
        SelectionGraphEdge* edge = node->getOutEdge(i);
        edges.erase(edge->getID());
    }

    //
    if(node != NULL)
        delete node;
}

SelectionGraphEdge* SelectionGraph::getEdgeByID(int edgeID){
    assert(edges.count(edgeID)>0);
    return edges[edgeID];
}

void SelectionGraph::removeEdge(int edgeID){
    assert(edges.count(edgeID) > 0);

    map<int,SelectionGraphEdge*>::iterator it = edges.find(edgeID);

    SelectionGraphEdge* edge = it->second;
    SelectionGraphNode* tail = edge->getTail();
    SelectionGraphNode* head = edge->getHead();

    assert(tail != NULL && head != NULL);

    edges.erase(it);
    delete edge; //the destructor removes the edge from the adjacency lists
    //of the nodes
}

string SelectionGraph::str(){
    stringstream ss;
    ss << "*** Graph Summary ***" << endl;
    ss << nodes.size() << " nodes" << endl;

    map<int,SelectionGraphNode*>::iterator itNodes;
    for(itNodes = nodes.begin() ; itNodes != nodes.end() ; ++itNodes){
        SelectionGraphNode* node = itNodes->second;
        ss << node->str() << endl;
    }

    ss << endl << edges.size() << " edges" << endl;
    ss << "    ";
    map<int,SelectionGraphEdge*>::iterator itEdges;
    ss << "    ";
    for(itEdges = edges.begin() ; itEdges != edges.end() ; ++itEdges){
        SelectionGraphEdge* edge = itEdges->second;
        ss << "e" << edge->getID() << "(" << edge->getTail()->getId() << " , "
           << edge->getHead()->getId() << ") ";
    }

    return ss.str();
}

int SelectionGraph::numberOfNodes(){
    return nodes.size();
}

int SelectionGraph::numberOfEdges(){
    return edges.size();
}

bool SelectionGraph::isEmpty(){
    return (numberOfNodes() == 0) && (numberOfEdges() == 0);
}

void SelectionGraph::groupNodesAndEdgeByColor(std::set<Group>& groups,
                                              std::map<Group, std::vector<SelectionGraphNode*> >& mapGroupToNodes,
                                              std::map<Group, std::vector<SelectionGraphEdge*> >& mapGroupToEdges){
    //
    groups.clear();
    mapGroupToNodes.clear();
    mapGroupToEdges.clear();

    //
    map<int,SelectionGraphNode*>::iterator beginNodeIterator;
    map<int,SelectionGraphNode*>::iterator   endNodeIterator;
    getNodeIterator(beginNodeIterator,endNodeIterator);

    for(;beginNodeIterator != endNodeIterator ; ++beginNodeIterator){
        SelectionGraphNode* node = beginNodeIterator->second;
        Group group = node->getGroup();
        groups.insert(group);

        if(mapGroupToNodes.count(group) == 0){
            vector<SelectionGraphNode*> nodes;
            nodes.push_back(node);
            mapGroupToNodes[group] = nodes;
        }
        else{
            vector<SelectionGraphNode*>& nodes = mapGroupToNodes[group];
            nodes.push_back(node);
        }
    }

    map<int,SelectionGraphEdge*>::iterator beginEdgeIterator;
    map<int,SelectionGraphEdge*>::iterator   endEdgeIterator;
    getEdgeIterator(beginEdgeIterator,endEdgeIterator);

    for(;beginEdgeIterator != endEdgeIterator ; ++beginEdgeIterator){
        SelectionGraphEdge* edge = beginEdgeIterator->second;
        Group group = edge->getGroup();
        groups.insert(group);

        if(mapGroupToEdges.count(group) == 0){
            vector<SelectionGraphEdge*> edges;
            edges.push_back(edge);
            mapGroupToEdges[group] = edges;
        }
        else{
            vector<SelectionGraphEdge*>& edges = mapGroupToEdges[group];
            edges.push_back(edge);
        }
    }

    //
    //make sure that every group has an entry
    set<Group>::iterator setIterator;
    for(setIterator = groups.begin() ; setIterator != groups.end() ; ++setIterator){
        Group g = *setIterator;
        if(mapGroupToNodes.count(g) == 0){
            vector<SelectionGraphNode*> tempVNode;
            mapGroupToNodes[g] = tempVNode;
        }
        if(mapGroupToEdges.count(g) == 0){
            vector<SelectionGraphEdge*> tempVEdge;
            mapGroupToEdges[g] = tempVEdge;
        }
    }


}
