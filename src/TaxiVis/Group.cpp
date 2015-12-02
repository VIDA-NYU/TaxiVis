#include "Group.h"
#include "GroupRepository.h"

/**********
  * Group *
  *********/

Group::Group()
{
    GroupRepository gp = GroupRepository::getInstance();
    *this = gp.getDefaultItem();
}

Group::Group(QColor color):
    color(color)
{}

bool Group::equals(const Group& g) const{
    return ((color.red()   == g.color.red()) &&
            (color.green() == g.color.green()) &&
            (color.blue()  == g.color.blue()));
}

const QColor &Group::getColor() const{
    return color;
}

bool Group::operator==(const Group& g) const{
    return this->equals(g);
}

bool Group::operator <(const Group& group) const{
    QColor tempColor = group.getColor();
    return ((color.red() < tempColor.red()) ||
            (color.red() == tempColor.red() && color.green() < tempColor.green()) ||
            (color.red() == tempColor.red() && color.green() == tempColor.green() &&
             color.blue() < tempColor.blue()) );
}
