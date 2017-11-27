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
    explicit DialogICCAD2017(QWidget *parent = 0);
    ~DialogICCAD2017();

private slots:
    void on_chooseLef_clicked();

    void on_chooseDef_clicked();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_chooseVerilog_clicked();

signals:
    void buildICCAD2017(std::string cells_lef, std::string tech_lef, std::string placed_def);

private:
    Ui::DialogICCAD2017 *ui;
};

#endif // DIALOGICCAD2017_H
