#ifndef DIALOGICCAD2017_H
#define DIALOGICCAD2017_H

#include <QDialog>
#include <QString>
#include <iostream>

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

namespace Ui {
class DialogICCAD2017;
}

class DialogICCAD2017 : public QDialog
{
    Q_OBJECT

public:
    //! DialogICCAD2017 Constructor
    /*!
       \brief Constructs a DialogICCAD2017.
     */
    explicit DialogICCAD2017(QWidget *parent = 0);

    //! DialogICCAD2017 Destructor
    /*!
       \brief Destroys the DialogICCAD2017, including its properties.
     */
    ~DialogICCAD2017();

private slots:
    //! Cell file choice button event
    /*!
       \brief Handles the event of choice of the cell file.
     */
    void on_cells_choose_clicked();

    //! Tech file choice button event
    /*!
       \brief Handles the event of choice of the tech file.
     */
    void on_tech_choose_clicked();

    //! Placed file choice button event
    /*!
       \brief Handles the event of choice of the placed file.
     */
    void on_placed_choose_clicked();

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
       \param cells_lef Cell file location.
       \param tech_lef Tech file location.
       \param placed_def Placed file location.
     */
    void buildICCAD2017(std::string cells_lef, std::string tech_lef, std::string placed_def);

private:
    Ui::DialogICCAD2017 *ui;
};

#endif // DIALOGICCAD2017_H
