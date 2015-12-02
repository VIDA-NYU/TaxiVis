#include "timeanalysiswidget.h"
#include "ui_timeanalysiswidget.h"
#include "clustering/pointclusterer.h"
#include "util/scalar.h"

#include <QDebug>
#include <QButtonGroup>

#include <iostream>
#include <sstream>

TimeAnalysisWidget::TimeAnalysisWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::TimeAnalysisWidget)
{
    _ui->setupUi(this);
    _ui->legendGroupBox->setLayout(new QFormLayout);

    // read data
    loadLowAndHighData(std::string(DATA_DIR) + "perDay.low");
    //
    initTransitionGraphs();

    //
    _ui->projectionWidget->setMainUI(this);
    _ui->projectionWidget->setRadiusFactor(_ui->radiusSlider->value());
    _ui->projectionWidget->setOpacity(_ui->opacitySlider->value());

    // Autoexclude the Selection Modes
    QButtonGroup *buttonGroup = new QButtonGroup;
    buttonGroup->addButton(_ui->singleSelection, 0);
    buttonGroup->addButton(_ui->groupSelection, 1);
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(selectionModeChanges(int)));

    //
    fillScalarComboBox();
    _selectedScalar = _scalars[0];
    _ui->colorByComboBox->setCurrentIndex(_selectedScalar->index());

    connect(_ui->colorByComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSelectedScalar(int)));
    connect(_ui->projectionWidget,SIGNAL(selectionChanged(QList<int>)), _ui->calendarWidget,SLOT(updateSelectedDays(QList<int>)));
    connect(_ui->projectionWidget,SIGNAL(selectionChanged(QList<int>)), _ui->graphWidget,SLOT(updateSelectedDays(QList<int>)));
    connect(_ui->calendarWidget, SIGNAL(changedSelectedDays(QList<int>)), _ui->projectionWidget, SLOT(updateSelectedPoints(QList<int>)));

    updateLegend();
}

TimeAnalysisWidget::~TimeAnalysisWidget()
{
    delete _ui;
}

Scalar *TimeAnalysisWidget::addScalar( std::string name ) {
    int index = -1;
    for ( unsigned i = 0; i < _scalars.size(); ++i )
        if ( _scalars[i]->name() == name ) {
            index = i;
            break;
        }

    if ( index == -1 ) {
        Scalar *scalar = new Scalar( name );
        scalar->setIndex(_scalars.size());
        _scalars.push_back( scalar );
        return scalar;
    } else {
        return _scalars[index];
    }
}

void TimeAnalysisWidget::changeSelectedScalar(int index)
{
    _selectedScalar = _scalars[index];
    updateLegend();
    repaint();
}

void TimeAnalysisWidget::on_applyButton_clicked()
{
    bool low = _ui->lowRadioButton->isChecked();
    int numClusters = _ui->numClustersSpin->value();
    std::vector<Point*> &points = low?_lowPoints.data():_highPoints.data();

    std::vector<Point> centers;
    std::vector<int> assignment;
    PointClusterer::kmeans(PointClusterer::RANDOM,numClusters,
                           PointClusterer::EUCLIDEAN,
                           points, 10, centers, assignment);

    std::ostringstream ss;
    ss << "K-means " << numClusters << (low?" [low]":" [high]");
    Scalar *scalar = addScalar(ss.str());

    // create labels
    scalar->labels().clear();
    for (unsigned i=0; i<centers.size(); ++i) {
        ss.str("");
        ss << "Cluster [" << i << "]";
        scalar->labels().push_back(ss.str());
    }

    // assigne scalar values to points
    for (unsigned i=0; i<assignment.size(); ++i) {
        Point *p = _lowPoints.data()[i];
        p->setScalarValue(scalar, assignment[i]);
    }

    // refill combox
    fillScalarComboBox();
    _ui->colorByComboBox->setCurrentIndex(scalar->index());
}

void TimeAnalysisWidget::updateLegend()
{
    std::vector<std::string> &labels = _selectedScalar->labels();

    // removinb previous legend
    QFormLayout *layout = (QFormLayout *)_ui->legendGroupBox->layout();
    QLayoutItem *child;
    while (layout->count()!=0 && (child = layout->takeAt(0)) != 0) {
        child->widget()->deleteLater();
        delete child;
    }

    for (unsigned i=0; i<labels.size(); ++i) {
        QLabel *colorLabel = new QLabel;
        colorLabel->setAutoFillBackground(true);
        colorLabel->setMinimumSize(70, 15);
        QPalette palette = colorLabel->palette();

        float denom = _selectedScalar->max() - _selectedScalar->min();
        float normalizedValue = ( i - _selectedScalar->min() ) / ( denom == 0 ? 1.f : denom );

        palette.setColor(colorLabel->backgroundRole(), _ui->projectionWidget->colorScale()->getColor(normalizedValue));
        colorLabel->setPalette(palette);

        layout->addRow(new QLabel(labels[i].c_str()), colorLabel);
    }

    repaint();
}

void TimeAnalysisWidget::loadLowAndHighData(std::string filename)
{
    std::string basename = filename.substr(0, filename.find_last_of('.'));    
    _highPoints.load(basename + ".high");

    //_lowPoints.load(basename + ".low");
    //_lowPoints.load(basename + ".low_normalized_cols");
//    _lowPoints.load(basename + ".low_normalized_rows");
    _lowPoints.load(basename + ".low_standarised");
    //_lowPoints.load(basename + ".low_unnormalized");

    // adding default scalars
    Scalar *noneScalar = addScalar("None");
    noneScalar->labels().push_back("None");

    Scalar *dayScalar = addScalar("Day");
    for (int i=0; i<7; ++i)
        dayScalar->labels().push_back(QDate::longDayName(i+1).toStdString());

    Scalar *wend_wdayScalar = addScalar("Weekend/Weekdays");
    wend_wdayScalar->labels().push_back("Weekdays");
    wend_wdayScalar->labels().push_back("Weekends");

    for (int i=0; i<_lowPoints.numPoints(); ++i) {
        Point *lp = _lowPoints.data()[i];
        Point *hp = _highPoints.data()[i];

        QDate d = lp->getDate();

        lp->setScalarValue(noneScalar, 0);
        lp->setScalarValue(dayScalar, d.dayOfWeek()-1);
        lp->setScalarValue(wend_wdayScalar, d.dayOfWeek()==6||d.dayOfWeek()==7);

        hp->setScalarValue(noneScalar, 0);
        hp->setScalarValue(dayScalar, d.dayOfWeek());
        hp->setScalarValue(wend_wdayScalar, d.dayOfWeek()==6||d.dayOfWeek()==7);
    }
}

void TimeAnalysisWidget::fillScalarComboBox()
{
    _ui->colorByComboBox->blockSignals(true);
    _ui->colorByComboBox->clear();
    for (unsigned i=0; i<_scalars.size(); ++i) {
        _ui->colorByComboBox->addItem(_scalars[i]->name().c_str());
    }
    _ui->colorByComboBox->blockSignals(false);
}

void TimeAnalysisWidget::initTransitionGraphs() {
    int numPoints = _highPoints.numPoints();
    for(int i = 0 ; i < numPoints ; ++i){
        NeighborhoodGraph graph(_highPoints.data().at(i)->coords(),28);
        graphs.push_back(graph);
    }

    _ui->graphWidget->setGraphRepository(&graphs);
}

void TimeAnalysisWidget::selectionModeChanges(int mode){
    if(mode == 0){
        _ui->projectionWidget->setSelectionMode(ProjectionViewWidget::SINGLE_SELECTION);
        _ui->graphWidget->selectionModeChanges(GraphSummaryContainerWidget::SINGLE_UPDATE);
    }
    else if(mode == 1){
        _ui->projectionWidget->setSelectionMode(ProjectionViewWidget::GROUP_SELECTION);
        _ui->graphWidget->selectionModeChanges(GraphSummaryContainerWidget::GROUP_UPDATE);
    }
}
