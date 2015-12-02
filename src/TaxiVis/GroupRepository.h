#ifndef GROUPREPOSITORY_H
#define GROUPREPOSITORY_H

#include "Group.h"
#include <vector>
#include <set>

class GroupRepository
{
private:
    static GroupRepository *instance;
private:
    std::vector<Group>  items;

public:
    GroupRepository();
    static GroupRepository& getInstance();
    const QColor&           getColor(int groupId) const;
    QColor                  getQColor(int groupId) const;
    void                    addItem(const QColor& color);
    int                     getNumItems() const;
    const Group&            getItem(int groupId) const;
    Group                   getDefaultItem();
    const Group&            getGroupByColor(const QColor& color);
    Group                   getNextAvailableGroup(std::set<Group> groups);
};

#endif // GROUPREPOSITORY_H
