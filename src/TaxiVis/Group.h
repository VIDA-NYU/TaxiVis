#ifndef GROUP_H
#define GROUP_H

#include <QColor>

class Group
{
private:
    std::string name;
    QColor color;
public:
    Group();
    Group(QColor color);
    bool equals(const Group& g) const;
    const QColor &getColor() const;
    bool operator==(const Group&) const;
    bool operator <(const Group& b) const;
};

#endif // GROUP_H
