#include "indigotabbar.h"
#include <QDebug>
#include <QStyle>
#include "helper.h"

IndigoTab::IndigoTab(QIcon icon, QWidget *parent) :
    QWidget(parent)
{
    ico_icon = icon;
    m_display = IndigoTab::visible;
}

void IndigoTab::setIcon(QIcon icon){
    ico_icon = icon;
}

QIcon IndigoTab::Icon(){
    return ico_icon;
}


void IndigoTab::setDisplayState(IndigoTab::Display display){
    m_display = display;
}

IndigoTab::Display IndigoTab::displayState(){
    return m_display;
}



/***************************************************************************
 * TODO:
 * - add overflow buttons
 *
 * ************************************************************************/

IndigoTabBar::IndigoTabBar(QWidget *parent) :
    QWidget(parent)
{

    int_tabHeight = 32;
    int_tabWidth = 32;
    int_hoverIndex = -1;
    int_gap = 1;
    int_borderHighlight = 2;
    dbl_transparency = 0.1; // 10%
    m_tabOrientation = IndigoTabBar::East;
    bool_dragProceed = false;
    int_dragIndex = -1;

    setAccessibleName("IndigoTabBar");
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Background);
    setFixedWidth(int_tabWidth);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));
    setMouseTracking(true);


}



void IndigoTabBar::mousePressEvent(QMouseEvent*event){

    if(lst_TabList.count() < 0) return;

    QPoint mouse = event->pos();

    int_hoverIndex = mouse.y() / (int_tabHeight + int_gap);
    int_oldIndex = realTabIndex(mouse.y());

    update();

}



void IndigoTabBar::mouseReleaseEvent(QMouseEvent*event){

    if(lst_TabList.count() < 0) return;

    QPoint mouse = event->pos();

    int_newIndex = realTabIndex(mouse.y());

    if(bool_dragProceed){
        bool_dragProceed = false;
        int_dragIndex = -1;
        moveTab();
    }else{
        // Scroll to position in DropZone after mouse click
        emit scrollToPanel(int_newIndex);
    }

    update();

}



void IndigoTabBar::mouseMoveEvent(QMouseEvent*event){


    if(lst_TabList.count() < 0) return;

    QPoint mouse = event->pos();

    int_hoverIndex = mouse.y() / (int_tabHeight + int_gap);

    // drag started
    if (event->buttons() == Qt::LeftButton) {
        bool_dragProceed = true;
        int_dragIndex = int_oldIndex;
        dragPosition = event->pos().y();

    }

    update();

}



void IndigoTabBar::leaveEvent(QEvent *event){

    Q_UNUSED(event)

    int_hoverIndex = -1;
    update();
}



void IndigoTabBar::paintEvent(QPaintEvent *event)
{
   Q_UNUSED(event)

    QPainter p(this);

    int i_visible = 0;

    for (int i = 0; i < lst_TabList.count(); ++i){


        int offset = 0;

        // draw visibile tabs
        IndigoTab *tab = lst_TabList.at(i+offset);

        if(tab->displayState() == IndigoTab::visible){


            QRect  tabRect(0,i_visible*(int_tabHeight + int_gap), int_tabWidth, int_tabHeight);
            QRect  gapRect(4,tabRect.y() - int_gap, int_tabWidth - 8, int_gap);

            QIcon icon = tab->Icon();
            QPixmap pix = icon.pixmap(QSize(int_tabWidth, int_tabHeight));


            // draw background / highlighter
            if(int_hoverIndex == i_visible && bool_dragProceed){


                col_colorHighlightAlpha = Helper().blendColor(QColor(this->palette().color(QPalette::Background)),
                                                   QColor(this->palette().color(QPalette::Highlight)),
                                                   dbl_transparency);

                p.fillRect(tabRect,
                           QColor(this->palette().color(QPalette::Highlight)));

                p.fillRect(tabRect.x()+int_borderHighlight,
                           tabRect.y()+int_borderHighlight,
                           tabRect.width() -(int_borderHighlight*2),
                           tabRect.height() -(int_borderHighlight*2),
                           col_colorHighlightAlpha);


            }else if(int_hoverIndex == i_visible){
                p.fillRect(tabRect, QColor(this->palette().color(QPalette::Base)));
            }

            // draw tab content
            if(int_dragIndex != i_visible){

                /*if(i_visible <= int_hoverIndex && dragProceed){
                    QRect offsetRect(0, tabRect.y()-(int_tabHeight + int_gap), int_tabWidth, int_tabHeight);
                    p.drawPixmap(offsetRect, pix );
                }else{
                    p.drawPixmap(tabRect, pix );
                }*/

                p.drawPixmap(tabRect, pix );

            }



            // draw divider
            p.fillRect(gapRect, Helper().blendColor(QColor(this->palette().color(QPalette::Background)),
                                             QColor(this->palette().color(QPalette::Base)),
                                             0.5));

            // count only visible tabs
            ++i_visible;
        }
    }


    // draw moveable Tab

    if(bool_dragProceed){

        if(int_dragIndex > lst_TabList.count() || int_dragIndex < 0) return;

        IndigoTab *tab = lst_TabList.at(int_dragIndex);

        QIcon icon = tab->Icon();
        QPixmap pix = icon.pixmap(QSize(int_tabWidth, int_tabHeight));

        QRect dragFrame(0, dragPosition, int_tabWidth, int_tabHeight);
       // p.fillRect(dragFrame, QColor(this->palette().color(QPalette::Base)));
        p.drawPixmap(dragFrame, pix );


    }


}



int IndigoTabBar::realTabIndex(int mouseY){

    int fakeIndex = fakeTabIndex(mouseY);

     for (int i = 0; i < lst_TabList.count(); ++i){

        // check if there a hidden tab before visible one
        if(lst_TabList.at(i)->displayState() == IndigoTab::hidden){
            ++fakeIndex;
        }

        // all hidden tabs are counted & realIndex was found
        if(fakeIndex == i){
            return i;
        }

    }

    return -1;
}



int IndigoTabBar::fakeTabIndex(int mouseY){

    // calculate fakeIndex by tab grid & mouse position
    int fakeIndex = mouseY / (int_tabHeight + int_gap);

    int visibleTabs = 0;

    // recalculate and limit index by real visible tabs
    for (int i = 0; i < lst_TabList.count(); ++i){

        // all visible tabs are counted & fakeIndex was found
        if(visibleTabs == fakeIndex || i == lst_TabList.count()-1){
            return visibleTabs;
        }

        // check if there a visibile tab on index
        if(lst_TabList.at(i)->displayState() == IndigoTab::visible){
            ++visibleTabs;           
        }
    }

    return -1;
}



void IndigoTabBar::addTab(QIcon icon){

    insertTab(icon, -1);
}



void IndigoTabBar::insertTab(QIcon icon, int index){

    IndigoTab *tab = new IndigoTab(icon);
    tab->setDisplayState(IndigoTab::visible);

    if(index == -1){       
        lst_TabList.append(tab);
    }else{
        lst_TabList.insert(index, tab);
    }

    calculateHeight();
    update();
}



void IndigoTabBar::removeTab(int index){

    if(index >= 0 && index <= lst_TabList.count()){
        lst_TabList.removeAt(index);
        calculateHeight();
        update();

    }

}



void IndigoTabBar::moveTab(){

    if(int_newIndex != int_oldIndex){

        IndigoTab *tab = lst_TabList.takeAt(int_oldIndex);
        lst_TabList.insert(int_newIndex, tab);

        emit tabMoved(int_oldIndex, int_newIndex);
        qDebug() << "TabMoved old" << int_oldIndex << "new" << int_newIndex << endl;
    }

}



void IndigoTabBar::hideTab(int index){

    if(index >= 0 && index <= lst_TabList.count()){
        lst_TabList.at(index)->setDisplayState(IndigoTab::hidden);
        calculateHeight();
        update();
    }
}



void IndigoTabBar::showTab(int index){

    if(index >= 0 && index <= lst_TabList.count()){
        lst_TabList.at(index)->setDisplayState(IndigoTab::visible);
        calculateHeight();
        update();

    }

}



void IndigoTabBar::setTabSize(int side){
    setTabSize(side, side);
}



void IndigoTabBar::setTabSize(int width, int height){
    int_tabWidth = width;
    int_tabHeight = height;
}



IndigoTabBar::Orientation IndigoTabBar::tabPosition(){
    return m_tabOrientation;
}



void IndigoTabBar::setTabPosition(Orientation tabOrientation){

    m_tabOrientation = tabOrientation;

}



int IndigoTabBar::currentIndex(){
    return int_newIndex;
}



void IndigoTabBar::calculateHeight(){

   int items=0;

   for( int i=0; i<lst_TabList.size(); ++i ){
       if(lst_TabList.at(i)->displayState() == IndigoTab::visible){

           items +=1;
       }

   }

   this->setFixedHeight(items*(int_tabHeight + int_gap));

}

