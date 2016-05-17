#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDialog>
#include <QMessageBox>

#include <QFileDialog>

#include "../geometry/geometry.h"



using namespace ophidian;

namespace uddac2016 {

void MainWindow::update_selected()
{
    bool enable = !m_selected.name().isEmpty();
   ui->combo_selected_type->setEnabled(enable);
   ui->line_edit_selected_x->setEnabled(enable);
   ui->line_edit_selected_y->setEnabled(enable);
   ui->combo_selected_type->clear();
   ui->label_selected_name->setText(m_selected.name());
   ui->combo_selected_type->addItems(m_selected.std_cells());
   ui->line_edit_selected_x->setDisabled(m_selected.fixed());
   ui->line_edit_selected_y->setDisabled(m_selected.fixed());
   ui->line_edit_selected_x->setText(QString::number(m_selected.position().x()));
   ui->line_edit_selected_y->setText(QString::number(m_selected.position().y()));
   ui->combo_selected_type->setCurrentText(m_selected.std_cell());
}

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_controller(*this)
{
    ui->setupUi(this);
    ui->menuBar->setController(m_controller);
    m_controller.init_canvas_controller(ui->Canvas);
    update_selected();

}

MainWindow::~MainWindow()
{

}

void MainWindow::circuit_name(const QString &name)
{
    ui->label_circuit_name->setText(name);
}

void MainWindow::die_area(const QPoint &size)
{
    ui->label_die_area->setText(QString::number(size.x()) + "x" + QString::number(size.y()));
}

void MainWindow::cell_count(int cells)
{
    ui->label_cell_count->setText(QString::number(cells));
}

void MainWindow::net_count(int nets)
{
    ui->label_net_count->setText(QString::number(nets));
}

void MainWindow::pin_count(int pins)
{
    ui->label_pin_count->setText(QString::number(pins));
}

void MainWindow::unselect()
{
    m_selected = selected_cell();
    update_selected();
}

void MainWindow::select(ophidian::entity_system::entity entity)
{
    unselect();
    m_selected = selected_cell(m_controller, entity);
    update_selected();
}


void MainWindow::on_combo_selected_type_activated(QString text)
{
    qDebug() << "changing size to " << text;
    if(!text.isEmpty())
        m_controller.cell_std_cell(m_selected.entity(), text);
}

void MainWindow::on_line_edit_selected_x_returnPressed()
{
    qDebug() << "enter X";
    m_controller.place_cell(m_selected.entity(), geometry::point<double>(ui->line_edit_selected_x->text().toDouble(), ui->line_edit_selected_y->text().toDouble()));
}

void MainWindow::on_line_edit_selected_y_returnPressed()
{
    qDebug() << "enter Y";
    m_controller.place_cell(m_selected.entity(), geometry::point<double>(ui->line_edit_selected_x->text().toDouble(), ui->line_edit_selected_y->text().toDouble()));
}

selected_cell::selected_cell():
    m_ctrl(nullptr)
{

}

selected_cell::selected_cell(controller &ctrl, ophidian::entity_system::entity entity) :
    m_ctrl(&ctrl),
    m_entity(entity)
{
}

QString selected_cell::name()
{
    if(!m_ctrl)
        return "";
    return m_ctrl->cell_name(m_entity);
}

QString selected_cell::std_cell()
{
    if(!m_ctrl)
        return "";
    return m_ctrl->cell_std_cell(m_entity);

}

QStringList selected_cell::std_cells()
{
    if(!m_ctrl)
        return QStringList();
    return m_ctrl->cell_std_cells(m_entity);
}

QPoint selected_cell::position()
{
    if(!m_ctrl)
        return QPoint(0, 0);
    auto position= m_ctrl->cell_position(m_entity);
    return QPoint(position.x(), position.y());
}

bool selected_cell::fixed()
{
    if(!m_ctrl)
        return true;
    return m_ctrl->cell_is_fixed(m_entity);
}


}

