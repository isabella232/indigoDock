#include "anglepicker.h"
#include "math.h"
#include <QStyle>
#include "qapplication.h"
#include "helper.h"


AnglePicker::AnglePicker(QWidget *parent)
    : QWidget(parent)
{
    //this->setFixedSize(30,30);

    int_diameter = 30;
    pnt_center = QPoint(this->int_diameter/2, this->int_diameter/2);
    pnt_pointer = QPoint(this->int_diameter/2, 0);
    int_lineThickness = 2;
    int_angle = 0;
    dbl_transparency = 0.55; // 10%

    rct_circleFrame = QRect(1,1,this->int_diameter-int_lineThickness - 1, this->int_diameter-int_lineThickness -1);

    this->setFixedSize(int_diameter, int_diameter);
}



void AnglePicker::paintEvent(QPaintEvent*) {

    QPainter painter(this);
    QColor c_line = QColor(this->palette().color(QPalette::WindowText));
    QColor c_fill = Helper().blendColor(QColor(this->palette().color(QPalette::Background)),
                                        QColor(this->palette().color(QPalette::Highlight)),
                                        dbl_transparency);

    QBrush brush(c_fill);

    QPen pen;
    pen.setColor(c_line);
    pen.setWidth(int_lineThickness);

    painter.setRenderHint(QPainter::Antialiasing);

    // draw filled angle
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawPie(rct_circleFrame, 0, 6000 / 360 * int_angle);
    painter.restore();

    painter.setPen(pen);

    // draw outer circle
    painter.drawEllipse(rct_circleFrame);

    // darw angle line
    painter.save();
    painter.translate(pnt_center.x(), pnt_center.y());
    painter.rotate(-int_angle);
    painter.drawLine(0, 0, int_diameter/2 - int_lineThickness - 1, 0);
    painter.restore();


}



void AnglePicker::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton)
    {
        pnt_pointer = event->pos();

        QLineF *line = new QLineF(pnt_center, pnt_pointer);

        // rotate 0° to 12 o'clock
        double angle = line->angle();// -90;
       // if (angle > 0) angle -= 360;

        // invert counter direction
        int_angle = angle;

        update();

        emit angleChanged();

        qDebug() << "mouseDown" << pnt_pointer << "Angle is:" << int_angle << endl;


    }
}



double AnglePicker::Angle() {

    return int_angle;
}



void AnglePicker::setAngle(double degree){

    if (degree < 0) degree = 0;
    if (degree > 360) degree = 360;

    int_angle = degree;

}
