#ifndef DIALOGICCAD2015_H
#define DIALOGICCAD2015_H

#include <QDialog>
#include <QString>
#include <iostream>

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

namespace Ui {
class DialogICCAD2015;
}

class DialogICCAD2015 : public QDialog
{
    Q_OBJECT

public:
    //! DialogICCAD2015 Constructor
    /*!
       \brief Constructs a DialogICCAD2015.
     */
    explicit DialogICCAD2015(QWidget *parent = 0);

    //! DialogICCAD2015 Destructor
    /*!
       \brief Destroys the DialogICCAD2015, including its properties.
     */
    ~DialogICCAD2015();

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

    void on_choose_early_lib_clicked();

    void on_choose_late_lib_clicked();

    void on_choose_sdc_clicked();

signals:
    //! Send files information
    /*!
       \brief Send path from files to main controller for design construction.
       \param lef Lef file location.
       \param def Deffile location.
       \param verilog Verilog file location.
     */
    void buildICCAD2015(std::string lef, std::string def, std::string verilog, std::string early, std::string late, std::string sdc);

private:
    Ui::DialogICCAD2015 *ui;
};

#endif // DIALOGICCAD2015_H
