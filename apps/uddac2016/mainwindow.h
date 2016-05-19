#ifndef OPHIDIAN_APPS_UDDAC2016_MAINWINDOW_H
#define OPHIDIAN_APPS_UDDAC2016_MAINWINDOW_H

#include <QMainWindow>
#include "controller.h"

namespace uddac2016 {

namespace Ui {
class MainWindow;
}

class selected_cell {
    controller * m_ctrl;
    ophidian::entity_system::entity m_entity;
public:
    selected_cell();
    selected_cell(controller & ctrl, ophidian::entity_system::entity entity);
    QString name();
    QString std_cell();
    QStringList std_cells();
    QPoint position();
    double worst_slack();
    bool fixed();

     ophidian::entity_system::entity entity() const {
         return m_entity;
     }

};


class MainWindow : public QMainWindow
{
    Q_OBJECT

    controller m_controller;
    selected_cell m_selected;

    void update_selected();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void circuit_name(const QString & name);
    void die_area(const QPoint & size);
    void cell_count(int cells);
    void net_count(int nets);
    void pin_count(int pins);


    void unselect();
    void select(ophidian::entity_system::entity entity);


private slots:
    void on_combo_selected_type_activated(QString text);
    void on_line_edit_selected_x_returnPressed();
    void on_line_edit_selected_y_returnPressed();
    void on_actionScreenshot_triggered();
    void on_lineEdit_name_regex_returnPressed();
    void on_list_visible_nets_itemSelectionChanged();
    void on_button_delete_visible_net_clicked();

private:
    Ui::MainWindow *ui;
};

}

#endif // OPHIDIAN_APPS_UDDAC2016_MAINWINDOW_H
