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
    ui->label_selected_slack->setText(QString::number(m_selected.worst_slack()));
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

const QColor &MainWindow::negativeSlackBegin() const
{
    return ui->menuBar->negativeSlackBegin();
}

const QColor &MainWindow::negativeSlackEnd() const
{
    return ui->menuBar->negativeSlackEnd();
}

const QColor &MainWindow::zeroSlack() const
{
    return ui->menuBar->zeroSlack();
}

const QColor &MainWindow::positiveSlack() const
{
    return ui->menuBar->positiveSlack();
}

void MainWindow::circuit_name(const QString &name)
{
    ui->label_circuit_name->setText(name);
}

void MainWindow::die_area(const QPoint &size)
{
    ui->label_die_area->setText(QString::number(size.x()) + "μm X " + QString::number(size.y()) + "μm");
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

bool MainWindow::isCriticalPathChecked()
{
    return ui->actionCritical_Path->isChecked();
}

void MainWindow::reset()
{
    ui->menuBar->reset();
}

void MainWindow::update_visible_nets()
{

    for(int row = 0; row < ui->list_visible_nets->count(); row++)
    {
        QListWidgetItem *item = ui->list_visible_nets->item(row);
        m_controller.remove_nets(item->text());
    }

    for(int row = 0; row < ui->list_visible_nets->count(); row++)
    {
        QListWidgetItem *item = ui->list_visible_nets->item(row);
        m_controller.show_nets(item->text());
    }
}

void MainWindow::repaintQuads()
{
    m_controller.repaint();
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

double selected_cell::worst_slack()
{
    if(!m_ctrl)
        return 0.0;
    return m_ctrl->cell_worst_slack(m_entity);
}

bool selected_cell::fixed()
{
    if(!m_ctrl)
        return true;
    return m_ctrl->cell_is_fixed(m_entity);
}

void MainWindow::on_actionScreenshot_triggered()
{
    m_controller.screenshot();
}

void MainWindow::on_lineEdit_name_regex_returnPressed()
{
    if(!ui->lineEdit_name_regex->text().isEmpty())
    {
        ui->list_visible_nets->addItem(ui->lineEdit_name_regex->text());
        auto matches = m_controller.show_nets(ui->lineEdit_name_regex->text());
    }
    ui->lineEdit_name_regex->clear();
}

void MainWindow::on_button_delete_visible_net_clicked()
{
    for(auto item : ui->list_visible_nets->selectedItems())
    {
        m_controller.remove_nets(item->text());
        delete item;
    }
}

void MainWindow::on_actionCritical_Path_toggled()
{
    qDebug() << ui->actionCritical_Path->isChecked();

    if(ui->actionCritical_Path->isChecked())
        m_controller.show_CP();
    else
        m_controller.clear_CP();

}


void MainWindow::on_list_visible_nets_itemSelectionChanged()
{
    ui->button_delete_visible_net->setDisabled(ui->list_visible_nets->selectedItems().isEmpty());
}


void MainWindow::on_actionShow_Cells_toggled(bool arg1)
{
    qDebug() << arg1;
    m_controller.show_cells(arg1);
}

}

