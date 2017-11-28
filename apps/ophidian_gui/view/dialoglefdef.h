#ifndef DIALOGLEFDEF_H
#define DIALOGLEFDEF_H

#include <QDialog>
#include <QString>
#include <iostream>

#include "ui_dialoglefdef.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

namespace Ui {
class DialogLefDef;
}

class DialogLefDef : public QDialog
{
    Q_OBJECT

public:
    //! DialogLefDef Constructor
    /*!
       \brief Constructs a DialogLefDef.
     */
    explicit DialogLefDef(QWidget *parent = 0);

    //! DialogLefDef Destructor
    /*!
       \brief Destroys the DialogLefDef, including its properties.
     */
    ~DialogLefDef();

private slots:
    //! Lef file choice button event
    /*!
       \brief Handles the event of choice of the lef file.
     */
    void on_chooseLef_clicked();

    //! Def file choice button event
    /*!
       \brief Handles the event of choice of the def file.
     */
    void on_chooseDef_clicked();

    //! Verilog file choice button event
    /*!
       \brief Handles the event of choice of the verilog file.
     */
    void on_chooseVerilog_clicked();

    //! Ok button event
    /*!
       \brief Verifies that all file fields are filled.
     */
    void on_okButton_clicked();

    //! Cancel button event
    /*!
       \brief Closes the dialog window.
     */
    void on_cancelButton_clicked();

signals:
    //! Send files information
    /*!
       \brief Send path from files to main controller for design construction.
       \param lef Lef file location.
       \param def Deffile location.
       \param verilog Verilog file location.
     */
    void buildICCAD2015(std::string lef, std::string def, std::string verilog);

private:
    Ui::DialogLefDef *ui;

};

#endif // DIALOGLEFDEF_H
