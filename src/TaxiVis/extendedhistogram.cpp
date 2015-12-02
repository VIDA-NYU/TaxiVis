#include "extendedhistogram.h"

ExtendedHistogram::ExtendedHistogram(QWidget *parent) :
    QCustomPlot(parent),
    currentState(IDLE),
    currentSelectionIndex(-1),
    x0(0),
    convertedValue0(0),
    x1(1),
    convertedValue1(1){}

void ExtendedHistogram::addSelection(double selMin, double selMax){
    IntervalSelection selection;
    selection.first = selMin;
    selection.second = selMax;
    selections.append(selection);

    QList<IntervalSelection> convertedSelections;//these should be used to query
    convertSelections(convertedSelections);
    emit updateSelection(convertedSelections);
}

void ExtendedHistogram::setControlValues(double x0, double convertedValue0, double x1, double convertedValue1){
    this->x0 = x0;
    this->convertedValue0 = convertedValue0;
    this->x1 = x1;
    this->convertedValue1 = convertedValue1;
}

void ExtendedHistogram::paintEvent(QPaintEvent *event){
    QCustomPlot::paintEvent(event);
    QPainter painter(this);

    QColor color = Qt::gray;
    color.setAlphaF(0.3);
    painter.setBrush(QBrush(color));

    //render stored selections
    IntervalSelection s;
    foreach(s, selections){
        double x     = xAxis->coordToPixel(s.first);
        double x_p_w = xAxis->coordToPixel(s.second);
        double x_avg = (x+x_p_w)/2;
        double y = 0;
        double h = height();
        painter.drawRect(QRectF(x,y,x_p_w - x,h));

        double convertedX0;
        double convertedX1;
        convertValue(s.first,convertedX0);
        convertValue(s.second,convertedX1);

        QString a = QString::number(convertedX0,'g',3);
        QString b = QString::number(convertedX1,'g',3);
        QString str = QString("[%1,%2]").arg(a).arg(b);

        painter.save();
        QPen blackThinPen(Qt::black);
        blackThinPen.setWidthF(0.6);
        painter.setPen(blackThinPen);
        QColor color = Qt::white;
        //color.setAlphaF(0.3);
        painter.setBrush(color);
        painter.drawRect(QRectF(x_avg-40,0,80,20));
        painter.restore();

        //
        painter.save();
        QPen redpen(Qt::black);//red);
        painter.setPen(redpen);
        painter.drawText(QRectF(x_avg-40,0,80,20),Qt::AlignHCenter,str);
        painter.restore();
    }

    //render current selection
    if(currentState == SELECTION){
        qreal x,y,w,h;
        bool currentIsMax = false;
        if(std::min(basePoint.x(),currentPoint.x()) == basePoint.x()){
            currentIsMax = true;
        }
        else{
            currentIsMax = false;
        }
        x = std::min(basePoint.x(),currentPoint.x());
        w = std::max(basePoint.x(),currentPoint.x()) - x;
        y = 0;
        h = height();

        //        QColor color = Qt::gray;
        //        color.setAlphaF(0.3);
        //        painter.setBrush(QBrush(color));
        painter.drawRect(QRectF(x,y,w,h));


        //
        double intervStart = xAxis->pixelToCoord(x);
        double tmp = intervStart;
        convertValue(tmp,intervStart);
        double intervEnd   = xAxis->pixelToCoord(x+w);
        tmp = intervEnd;
        convertValue(tmp,intervEnd);

        if(currentIsMax){
            painter.drawText(x,basePoint.y(),50,20,0,QString::number(intervStart,'g',3));
            painter.drawText(x+w,currentPoint.y() + 20,50,20,0,QString::number(intervEnd,'g',3));
        }
        else{
            painter.drawText(x,currentPoint.y() + 20,50,20,0,QString::number(intervStart,'g',3));
            painter.drawText(x+w,basePoint.y(),50,20,0,QString::number(intervEnd,'g',3));
        }
    }
}

void ExtendedHistogram::pickSelection(QPointF p, bool& pickedOne, int &index){
    double x = xAxis->pixelToCoord(p.x());
    pickedOne = false;
    index     = -1;

    QList<IntervalSelection>::iterator it;
    for(it = selections.begin() ; it != selections.end() ; ++it){
        IntervalSelection s = *it;
        index = index + 1;
        if(s.first <= x && x <= s.second){
            pickedOne = true;
            return;
            //qDebug() << "   Picked Selection " << index;
        }
    }
}

void ExtendedHistogram::convertValue(const double x, double &value){
    float lambda = (x - x0)/(x1-x0);
    value = convertedValue0 + lambda * (convertedValue1 - convertedValue0);
}

void ExtendedHistogram::inverseConvertValue(const double x, double &value){
    float lambda = (x - convertedValue0)/(convertedValue1-convertedValue0);
    value = x0 + lambda * (x1 - x0);
}

void ExtendedHistogram::convertSelections(QList<IntervalSelection> &convertedSelections){
    convertedSelections.clear();
    QList<IntervalSelection>::iterator it;
    for(it = selections.begin() ; it != selections.end() ; ++it){
        IntervalSelection sel = *it;
        IntervalSelection convertedSel;
        double x;
        convertValue(sel.first,x);
        convertedSel.first = x;
        double y;
        convertValue(sel.second,y);
        convertedSel.second = y;
        convertedSelections.append(convertedSel);
    }
}

void ExtendedHistogram::convertSelection(const double &lowerBound, const double &upperBound, IntervalSelection &sel){
    double x;
    convertValue(lowerBound,x);
    sel.first = x;
    double y;
    convertValue(upperBound,y);
    sel.second = y;
}

void ExtendedHistogram::inverseConvertSelection(const double &lowerBound, const double &upperBound, IntervalSelection &sel){
    double x;
    inverseConvertValue(lowerBound,x);
    sel.first = x;
    double y;
    inverseConvertValue(upperBound,y);
    sel.second = y;
}

QList<IntervalSelection> ExtendedHistogram::getSelections()
{
  QList<IntervalSelection> selections;
  this->convertSelections(selections);
  return selections;
}

void ExtendedHistogram::clearSelections(){
    this->selections.clear();
}

void ExtendedHistogram::mousePressEvent(QMouseEvent *event){
    //qDebug() << "Click";
    basePoint    = event->posF();
    lastPoint    = basePoint;
    currentPoint = basePoint;

    //pickSelection
    int index = -1;
    bool pickedSelection = false;
    //qDebug() << "Pick Selection " << event->posF();
    pickSelection(event->posF(),pickedSelection,index);

    if(pickedSelection){
        if(event->modifiers() & Qt::CTRL){
            selections.removeAt(index);

            QList<IntervalSelection> convertedSelections;//these should be used to query
            convertSelections(convertedSelections);

            currentState = IDLE;
            emit updateSelection(convertedSelections);
        }
        else if(event->button() == Qt::LeftButton){//drag it
            currentState = DRAG;
            currentSelectionIndex = index;
        }
        else if(event->button() == Qt::RightButton){//scale it
            currentState = SCALE;
            currentSelectionIndex = index;
        }
    }
    else if(event->button() & Qt::RightButton){
        if(event->modifiers() & Qt::SHIFT){
            double lowerBound = QInputDialog::getDouble(this,"Selection Lower Bound", "Lower Bound");
            double upperBound = QInputDialog::getDouble(this, "Selection Upper Bound", "Upper Bound");
            IntervalSelection sel;
            inverseConvertSelection(lowerBound, upperBound, sel);
            this->addSelection(sel.first,sel.second);

            currentState = IDLE;
        }
        else
            currentState = SELECTION;
    }
    else
        QCustomPlot::mousePressEvent(event);
}

void ExtendedHistogram::mouseMoveEvent(QMouseEvent *event){
    currentPoint = event->posF();
    double lastCoord = xAxis->pixelToCoord(lastPoint.x());
    double currentCoord = xAxis->pixelToCoord(currentPoint.x());
    double delta = currentCoord - lastCoord;

    if(currentState == SELECTION){

    }
    else if(currentState == DRAG){
        //qDebug() << "Dragging selection " << currentSelectionIndex;
        IntervalSelection &selection = selections[currentSelectionIndex];
        selection.first = selection.first + delta;
        selection.second = selection.second + delta;
    }
    else if(currentState == SCALE){
        //qDebug() << "Dragging selection " << currentSelectionIndex;
        IntervalSelection &selection = selections[currentSelectionIndex];
        if(selection.first - delta < selection.second + delta){
            selection.first = selection.first - delta;
            selection.second = selection.second + delta;
        }
    }
    else
        QCustomPlot::mouseMoveEvent(event);

    lastPoint = currentPoint;

    repaint();
}

void ExtendedHistogram::mouseReleaseEvent(QMouseEvent *event){
    if(currentState == SELECTION){
        qreal x,y,w,h;
        x = std::min(basePoint.x(),currentPoint.x());
        y = 0;
        w = std::max(basePoint.x(),currentPoint.x()) - x;
        h = height();

        if(x != x+w){
            double intervStart = xAxis->pixelToCoord(x);
            double intervEnd   = xAxis->pixelToCoord(x+w);
            IntervalSelection selection = std::make_pair(intervStart,intervEnd);
            selections.append(selection);
        }
    }

    if(currentState == SELECTION || currentState == DRAG || currentState == SCALE){
        //UPDATE QUERY

        QList<IntervalSelection> convertedSelections;//these should be used to query
        convertSelections(convertedSelections);
        emit updateSelection(convertedSelections);
    }

    currentState = IDLE;
    QCustomPlot::mouseReleaseEvent(event);
    repaint();
}
