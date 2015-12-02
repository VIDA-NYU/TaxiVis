#ifndef TIMEANALYSISWIDGET_H
#define TIMEANALYSISWIDGET_H

#include <QWidget>
#include "projectionView/pointset.h"

//
#include "neighborhoodgraph.h"
#include <QDate>

namespace Ui {
class TimeAnalysisWidget;
}
class Scalar;

class TimeAnalysisWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TimeAnalysisWidget(QWidget *parent = 0);
    ~TimeAnalysisWidget();
    
    Scalar *addScalar( std::string name );

    // GETTERS
    PointSet &lowPoints() { return _lowPoints; }
    PointSet &highPoints() { return _highPoints; }
    std::vector<Scalar *> &scalars() { return _scalars; }
    Scalar *selectedScalar() { return _selectedScalar; }


public slots:
    void changeSelectedScalar(int);
    void on_applyButton_clicked();
    void updateLegend();

private:
    void loadLowAndHighData(std::string filename);
    void fillScalarComboBox();

    void initTransitionGraphs();

private slots:
    void selectionModeChanges(int);
private:
    Ui::TimeAnalysisWidget *_ui;

    //
    std::vector<Scalar *>   _scalars;
    PointSet                _lowPoints;
    PointSet                _highPoints;
    Scalar                 *_selectedScalar;

    //
    std::vector<NeighborhoodGraph> graphs;
};

#endif // TIMEANALYSISWIDGET_H
