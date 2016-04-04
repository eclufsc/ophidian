#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->canvas->main_window(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::select_cell(openeda::entity::entity cell)
{
    std::string cell_name = m_app->netlist().cell_name(cell);
    ui->label_cell_name->setText(QString::fromStdString(cell_name));

    openeda::entity::entity cell_std_cell = m_app->netlist().cell_std_cell(cell);
    std::string std_cell_name = m_app->std_cells().cell_name(cell_std_cell);
    ui->combo_box_cell_type->setCurrentText(QString::fromStdString(std_cell_name));

    auto position = m_app->placement().cell_position(cell);
    ui->label_X->setText(QString::number(position.x()));
    ui->label_Y->setText(QString::number(position.y()));

    boost::units::quantity< boost::units::si::time >  worst_slack = m_app->cell_worst_slack(cell);
    ui->label_worst_slack->setText(QString::number(worst_slack.value()));


}

void MainWindow::load_circuit()
{
    for (auto std_cell_name : m_app->std_cells().cell_names()) {
        ui->combo_box_cell_type->addItem(QString::fromStdString(std_cell_name));
    }
}

void MainWindow::on_actionOpen_Circuit_triggered()
{
    if(!m_open_dialog)
        m_open_dialog.reset(new open_circuit_dialog(m_app, ui->canvas, this));
    m_open_dialog->show();
}

void MainWindow::on_combo_box_cell_type_activated(const QString &current_text)
{
    std::string cell_name = ui->label_cell_name->text().toStdString();
    auto cell = m_app->netlist().cell_find(cell_name);
    std::string std_cell_name = current_text.toStdString();
    if (!m_app->cell_std_cell(cell, std_cell_name)) {
        QMessageBox error_message;
        error_message.information(0, "Error", "Invalid cell type!");
        openeda::entity::entity cell_std_cell = m_app->netlist().cell_std_cell(cell);
        std_cell_name = m_app->std_cells().cell_name(cell_std_cell);
        ui->combo_box_cell_type->setCurrentText(QString::fromStdString(std_cell_name));
    } else {
        ui->canvas->update_cell(cell);
    }

}

void MainWindow::on_button_sta_clicked()
{
    ui->canvas->run_sta();
}
