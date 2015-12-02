#include "GroupRepository.h"
#include<iostream>
#include<sstream>
#include<fstream>
#include<cassert>

using namespace std;

GroupRepository* GroupRepository::instance = 0;

GroupRepository &GroupRepository::getInstance()
{
    if (instance == 0)
    {
        instance = new GroupRepository();

        string colorsFile(string(RESOURCES_DIR) + "qualitative.colors");
        ifstream f(colorsFile.c_str());
        if (f.is_open())
        {
          //cout << "Reading colormap file." << endl;
            string line;
            while(getline(f, line ) )
            {
                int r,g,b;
//                float coef = 0.4;
                if (sscanf(line.c_str(),"%d %d %d",&r,&g,&b) == 3)
                {
                    QColor color(r,g,b);
                    //assert(color != Qt::black);
                    instance->addItem(color);
//                    instance->addItem(QColor(coef*r + (1.0-coef),
//                                                   coef*g + (1.0-coef),
//                                                   coef*b + (1.0-coef)));
                }
            }    
        }
        else
            cerr << "Could not open " << colorsFile << endl;
    }
    return *instance;
}

GroupRepository::GroupRepository()
{}

const QColor &GroupRepository::getColor(int groupId) const
{
    assert (groupId >= 0 && groupId < static_cast<int>(items.size()));
    const Group &item = items[groupId];
    return item.getColor();
}

QColor GroupRepository::getQColor(int groupId) const
{
    const QColor &color = getColor(groupId);
    return color;
}

void GroupRepository::addItem(const QColor &color)
{
    items.push_back(Group(color));
}

int GroupRepository::getNumItems() const
{
    return items.size();
}

const Group &GroupRepository::getItem(int groupId) const
{
    return items[groupId];
}

Group GroupRepository::getDefaultItem(){
    return items[0];
}

const Group &GroupRepository::getGroupByColor(const QColor& color){
    int numberOfItems = items.size();

    for(int i = 0 ; i < numberOfItems ; ++i){
        if(color == items.at(i).getColor())
            return items.at(i);
    }

    // TODO: It needs to be corrected;
    return *(new Group());
}

Group GroupRepository::getNextAvailableGroup(std::set<Group> groups){
    int numberOfItems = items.size();

    for(int i = 1; i < numberOfItems ; ++i){
        Group& group =  items.at(i);
        if(groups.count(group) == 0)
            return group;
    }

    return items.at(0);
}
