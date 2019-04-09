#ifndef PLUGIN_MANAGER2_H
#define PLUGIN_MANAGER2_H

#include <QWidget>

namespace Ui {
class plugin_Manager2;
}

class plugin_Manager2 : public QWidget
{
    Q_OBJECT

public:
    explicit plugin_Manager2(QWidget *parent = 0);
    ~plugin_Manager2();

private:
    Ui::plugin_Manager2 *ui;
};

#endif // PLUGIN_MANAGER2_H
