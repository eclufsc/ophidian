#ifndef MAINWINDOWMENUBAR_H
#define MAINWINDOWMENUBAR_H

#include <QMenuBar>
#include "mainwindow.h"
#include "controller.h"

#include "preferencesdialog.h"


namespace uddac2016 {

class PreferencesDialog;
class MainWindowMenuBar : public QMenuBar
{
    Q_OBJECT
    MainWindow & m_mainwindow;
    controller * m_ctrl;

    std::map<std::string, QAction*> m_actions;

    PreferencesDialog m_preferences;

    bool m_lef_def{false};
    bool m_verilog{false};
    bool m_timing_lib{false};

    void update();

public:
    explicit MainWindowMenuBar(QMainWindow * parent = 0);
    virtual ~MainWindowMenuBar();

    void setController(controller& ctrl);


    const QColor & negativeSlackBegin() const {
        return m_preferences.negativeSlackBegin();
    }

    const QColor & negativeSlackEnd() const {
        return m_preferences.negativeSlackEnd();
    }

    const QColor & zeroSlack() const {
        return m_preferences.zeroSlack();
    }

    const QColor & positiveSlack() const {
        return m_preferences.positiveSlack();
    }

public slots:
    void action_open_LEFDEF_triggered();
    void action_open_placement_solution_triggered();
    void LEFDEF_accept(QString LEF, QString DEF);
    void LEFDEF_reject();
    void action_open_verilog_triggered();
    void action_run_SA_triggered();
    void action_run_STA_triggered();
    void action_open_timing_library_triggered();
    void openPreferencesWindow();
};

}

#endif // MAINWINDOWMENUBAR_H
