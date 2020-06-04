#include <QtCore/QObject>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QWidget>

class TestMouseEvent : public QWidget
{
public:
    TestMouseEvent();
    void doSomethingWith(const QPoint &) { }
    void mouseMoveEvent(QMouseEvent *event) override {
        doSomethingWith(event->pos()); // Warn
    }
};

