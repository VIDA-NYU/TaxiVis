#ifndef TAXIVIS_SELECTION_SNAPSHOT_HPP
#define TAXIVIS_SELECTION_SNAPSHOT_HPP
#include "SelectionGraph.h"
#include "KdTrip.hpp"
#include <map>
#include <vector>

// Selection geometry is flattened to polygons at capture time, on the GUI
// thread. QPainterPath is implicitly shared and QPainterPath::contains()
// populates a lazily computed bounds cache through a const method, so handing
// path copies to workers would race with the GUI thread (and with each other)
// on that cache while the owning Selection is still live. QPolygonF has no
// such cache. Selections are built only from addRect/addPolygon/moveTo/lineTo,
// so toFillPolygon() is exact here rather than an approximation of curves.
struct Region {
    QPolygonF outline;
    Qt::FillRule fill = Qt::OddEvenFill;
    static Region from(const QPainterPath &path) {
        Region region;
        region.outline = path.toFillPolygon();
        region.fill = path.fillRule();
        return region;
    }
    bool contains(const QPointF &point) const { return outline.containsPoint(point, fill); }
    // Computed from the points on each call; QPolygonF keeps no cached bounds,
    // which is what makes this safe to call from a worker.
    QRectF boundingRect() const { return outline.boundingRect(); }
};

struct SpatialConstraint {
    Region pickup, dropoff;
    bool hasPickup = false, hasDropoff = false;
    bool matches(const KdTrip::Trip *trip) const {
        return (!hasPickup || pickup.contains(QPointF(trip->pickup_lat, trip->pickup_long))) &&
               (!hasDropoff || dropoff.contains(QPointF(trip->dropoff_lat, trip->dropoff_long)));
    }
};
struct SelectionSnapshot {
    bool global = true;
    std::map<Group, std::vector<SpatialConstraint>> groups;
    static SelectionSnapshot capture(SelectionGraph *graph) {
        SelectionSnapshot out;
        if (!graph || graph->isEmpty()) {
            out.groups[Group(Qt::black)];
            return out;
        }
        out.global = false;
        std::set<int> linked;
        SelectionGraph::EdgeIterator eb, ee;
        graph->getEdgeIterator(eb, ee);
        for (; eb != ee; ++eb) {
            auto edge = eb->second;
            SpatialConstraint c;
            c.hasPickup = c.hasDropoff = true;
            c.pickup = Region::from(edge->getTail()->getSelection()->getGeometry());
            c.dropoff = Region::from(edge->getHead()->getSelection()->getGeometry());
            out.groups[edge->getGroup()].push_back(c);
            linked.insert(edge->getTail()->getId());
            linked.insert(edge->getHead()->getId());
        }
        SelectionGraph::NodeIterator nb, ne;
        graph->getNodeIterator(nb, ne);
        for (; nb != ne; ++nb) {
            auto node = nb->second;
            if (linked.count(node->getId())) continue;
            auto selection = node->getSelection();
            auto &constraints = out.groups[node->getGroup()];
            if (selection->getType() != Selection::END) {
                SpatialConstraint c;
                c.hasPickup = true;
                c.pickup = Region::from(selection->getGeometry());
                constraints.push_back(c);
            }
            if (selection->getType() != Selection::START) {
                SpatialConstraint c;
                c.hasDropoff = true;
                c.dropoff = Region::from(selection->getGeometry());
                constraints.push_back(c);
            }
        }
        return out;
    }
    bool matches(const Group &group, const KdTrip::Trip *trip) const {
        if (global) return true;
        const auto &constraints = groups.at(group);
        for (const auto &c : constraints) if (c.matches(trip)) return true;
        return false;
    }
};
#endif
