#ifndef SELECTIONGRAPH_H
#define SELECTIONGRAPH_H

#include "Selection.h"
#include "Group.h"
#include <set>

class SelectionGraphEdge;

class SelectionGraphNode{
private:
    int id;
    std::vector<SelectionGraphEdge*> inEdges;
    std::vector<SelectionGraphEdge*> outEdges;
    Selection                        *selection;
    Group                            group;

    //
    bool                             selected;//used for gui interaction

    //
    static int nextNodeID;
public:
    SelectionGraphNode();
    SelectionGraphNode(Selection*,Group group);
    SelectionGraphNode(SelectionGraphNode*);
    ~SelectionGraphNode();
    int                  getId();
    int                  inDegree();
    SelectionGraphEdge*  getInEdge(int index);
    int                  outDegree();
    SelectionGraphEdge*  getOutEdge(int index);
    std::string          str();
    void                 addInEdge(SelectionGraphEdge*);//TODO:Ensure that the graph is simple
    void                 addOutEdge(SelectionGraphEdge*);//TODO:Ensure that the graph is simple
    void                 removeInEdge(int edgeID);
    void                 removeOutEdge(int edgeID);
    Selection*           getSelection();
    Group                getGroup();
    void                 setGroup(const Group &);
    void                 copyGroupToNeighbors();
    //
    void                 setSelected(bool);
    void                 toogleSelected();
    bool                 isSelected();
};

class SelectionGraphEdge{
private:
    int                 id;
    SelectionGraphNode* tail;
    SelectionGraphNode* head;
    Group               group;

    //
    bool                             selected;//used for gui interaction

    //
    static int nextEdgeID;
public:
    SelectionGraphEdge(SelectionGraphNode* tail,SelectionGraphNode* head,Group group);
    ~SelectionGraphEdge();
    SelectionGraphNode* getTail();
    SelectionGraphNode* getHead();
    SelectionGraphNode* opposite(SelectionGraphNode*);
    int                 getID();
    std::string         std();
    Group               getGroup();
    void                setGroup(Group group);
    //
    void                 setSelected(bool);
    void                 toogleSelected();
    bool                 isSelected();
};

class SelectionGraph{
private:
    std::map<int,SelectionGraphNode*>  nodes;
    std::map<int,SelectionGraphEdge*> edges;
public:
public:
    typedef std::map<int,SelectionGraphNode*>::iterator         NodeIterator;
    typedef std::map<int,SelectionGraphNode*>::reverse_iterator NodeReverseIterator;
    typedef std::map<int,SelectionGraphEdge*>::iterator         EdgeIterator;
public:
    SelectionGraph();
    ~SelectionGraph();
    SelectionGraphNode* addNode();
    SelectionGraphNode* addNode(Selection *);
    void                addNode(SelectionGraphNode *);
    SelectionGraphEdge* addEdge();
    SelectionGraphEdge* addEdge(SelectionGraphNode* tail,SelectionGraphNode* head);
    void assign(SelectionGraph *);

    SelectionGraphNode* getNodeByID(int nodeID);
    void getNodeIterator(NodeIterator& begin, NodeIterator& end);
    void getNodeReverseIterator(NodeReverseIterator& begin,NodeReverseIterator& end);

    void getEdgeIterator(EdgeIterator& begin, EdgeIterator& end);

    void                removeNode(int nodeID);

    SelectionGraphEdge* getEdgeByID(int edgeID);
    void                removeEdge(int edgeID);
    std::string         str();

    int                 numberOfNodes();
    int                 numberOfEdges();

    bool                isEmpty();

    //
    void                groupNodesAndEdgeByColor(std::set<Group>& groups,
                                                 std::map<Group, std::vector<SelectionGraphNode*> >& mapGroupToNodes,
                                                 std::map<Group, std::vector<SelectionGraphEdge*> >& mapGroupToEdges);
};

#endif // SELECTIONGRAPH_H
