#include "mainwindowmenubar.h"

#include "openlefdefdialog.h"
#include <QDebug>
#include <QMessageBox>

#include <QFileDialog>

namespace uddac2016 {

void MainWindowMenuBar::update()
{
    if(m_lef_def)
    {
        m_actions.at("read_sol")->setEnabled(true);
        m_actions.at("verilog")->setEnabled(true);
        m_actions.at("timing_lib")->setEnabled(true);
    }

    if(m_verilog)
        m_actions.at("run_SA")->setEnabled(true);

    if(m_timing_lib && m_verilog)
        m_actions.at("STA")->setEnabled(true);


}

MainWindowMenuBar::MainWindowMenuBar(QMainWindow *parent) :
    QMenuBar(parent),
    m_mainwindow(*dynamic_cast<MainWindow*>(parent))
{
    QMenu * file = this->addMenu("File");
    QAction * action = file->addAction("Open LEF/DEF...");
    connect(action, SIGNAL(triggered()), this, SLOT(action_open_LEFDEF_triggered()));

    QAction * read_sol = file->addAction("Open placement solution (.DEF)...");
    connect(read_sol, SIGNAL(triggered()), this, SLOT(action_open_placement_solution_triggered()));
    read_sol->setEnabled(false);

    QAction * exit = file->addAction("Exit");
    connect(exit, SIGNAL(triggered()), &m_mainwindow, SLOT(close()));

    QMenu * netlistMenu = this->addMenu("Netlist");
    QAction * openVerilogAction = netlistMenu->addAction("Open Verilog File...");
    connect(openVerilogAction, SIGNAL(triggered()), this, SLOT(action_open_verilog_triggered()));
    openVerilogAction->setDisabled(true);

    QMenu * optimizationMenu = this->addMenu("Optimization");
    QAction * runSAAction = optimizationMenu->addAction("Run Simulated Annealing Placement");
    connect(runSAAction, SIGNAL(triggered()), this, SLOT(action_run_SA_triggered()));
    runSAAction->setDisabled(true);

    QMenu * timingMenu = this->addMenu("Timing");
    QAction * readLibraryAction = timingMenu->addAction("Open Timing Library");
    QAction * runSTAAction = timingMenu->addAction("Run Static Timing Analysis");

    connect(readLibraryAction, SIGNAL(triggered()), this, SLOT(action_open_timing_library_triggered()));
    connect(runSTAAction, SIGNAL(triggered()), this, SLOT(action_run_STA_triggered()));
    readLibraryAction->setDisabled(true);
    runSTAAction->setDisabled(true);

    m_actions["file"] = action;
    m_actions["read_sol"] = read_sol;
    m_actions["exit"] = exit;
    m_actions["verilog"] = openVerilogAction;
    m_actions["run_SA"] = runSAAction;
    m_actions["timing_lib"] = readLibraryAction;
    m_actions["STA"] = runSTAAction;
}

MainWindowMenuBar::~MainWindowMenuBar()
{

}

void MainWindowMenuBar::setController(controller &ctrl)
{
    m_ctrl = &ctrl;
}

void MainWindowMenuBar::action_open_LEFDEF_triggered()
{
    OpenLEFDEFDialog dialog(&m_mainwindow);
    connect(&dialog, SIGNAL(accepted(QString, QString)), this, SLOT(LEFDEF_accept(QString, QString)));
    connect(&dialog, SIGNAL(rejected()), this, SLOT(LEFDEF_reject()));
    dialog.exec();
}

void MainWindowMenuBar::action_open_placement_solution_triggered()
{
    auto text = QFileDialog::getOpenFileName(this, tr("Open DEF File..."), "", tr("Design Exchange Format (*.def)"));
    if(!text.isEmpty())
        m_ctrl->read_def(text.toStdString());
}

void MainWindowMenuBar::LEFDEF_accept(QString LEF, QString DEF)
{
    qDebug() << "accepted("<< LEF<<", " << DEF << ")";
    if(!m_ctrl->read_lefdef(LEF.toStdString(), DEF.toStdString()))
        QMessageBox::critical(this, "Error!", "Error reading LEF/DEF!!");
    else
        m_lef_def = true;
    update();

}

void MainWindowMenuBar::LEFDEF_reject()
{
    qDebug() << "rejected()";
}

void MainWindowMenuBar::action_open_verilog_triggered()
{
    auto text = QFileDialog::getOpenFileName(this, tr("Open Verilog File..."), "", tr("Verilog (*.v)"));
    if(!text.isEmpty())
    {
        m_ctrl->read_verilog(text.toStdString());
        m_verilog = true;
    }
    update();
}

void MainWindowMenuBar::action_run_SA_triggered()
{
    m_ctrl->run_SA();
}

void MainWindowMenuBar::action_run_STA_triggered()
{
    m_ctrl->run_STA();
}

void MainWindowMenuBar::action_open_timing_library_triggered()
{
    auto text = QFileDialog::getOpenFileName(this, tr("Open Timing Library File..."), "", tr("Timing Library (*.tau2014 *.lib)"));
    if(!text.isEmpty())
    {
        if(text.endsWith(".tau2014"))
        {
            m_ctrl->read_tau2014_lib(text.toStdString());
        }
        else if(text.endsWith(".lib"))
        {
            m_ctrl->read_liberty(text.toStdString());
        }
        else{
        }
        m_timing_lib = true;
    }
    update();

}

}
