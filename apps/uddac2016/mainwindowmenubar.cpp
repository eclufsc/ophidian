#include "mainwindowmenubar.h"

#include "openlefdefdialog.h"
#include <QDebug>
#include <QMessageBox>

#include <QFileDialog>

namespace uddac2016 {

MainWindowMenuBar::MainWindowMenuBar(QMainWindow *parent) :
    QMenuBar(parent),
    m_mainwindow(*dynamic_cast<MainWindow*>(parent))
{
    QMenu * file = this->addMenu("File");
    QAction * action = file->addAction("Open LEF/DEF...");
    connect(action, SIGNAL(triggered()), this, SLOT(action_open_LEFDEF_triggered()));

    QAction * read_sol = file->addAction("Open placement solution (.DEF)...");
    connect(read_sol, SIGNAL(triggered()), this, SLOT(action_open_placement_solution_triggered()));


    QAction * exit = file->addAction("Exit");
    connect(exit, SIGNAL(triggered()), &m_mainwindow, SLOT(close()));
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
}

void MainWindowMenuBar::LEFDEF_reject()
{
    qDebug() << "rejected()";
}

}
