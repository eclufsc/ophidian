#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QColorDialog>
#include <QDebug>

namespace uddac2016 {

void PreferencesDialog::draw()
{
    update(ui->negativeSlackColorRangeBeginLineEdit, m_negativeSlackBegin);
    update(ui->negativeSlackColorRangeEndLineEdit, m_negativeSlackEnd);
    update(ui->zeroSlackColorLineEdit, m_zeroSlack);
    update(ui->positiveSlackColorLineEdit, m_positiveSlack);
}

void PreferencesDialog::update(QLineEdit *lineedit, const QColor &color)
{
    lineedit->setText(color.name());
    lineedit->setStyleSheet("color: "+ color.name() + ";");
}

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    setWindowTitle("Preferences");

    m_negativeSlackBegin = QColor::fromHsv(0, .7*255, .9*255);
    m_negativeSlackEnd = QColor::fromHsv(60, .7*255, .9*255);
    m_zeroSlack = QColor::fromHsv(140, .7*255, .9*255);
    m_positiveSlack = QColor::fromHsv(230, .7*255, .9*255);

    draw();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_negativeSlackColorRangeBeginPickColorButton_clicked()
{
    QColor current;
    current.setNamedColor(ui->negativeSlackColorRangeBeginLineEdit->text());
    QColor color = QColorDialog::getColor(current);
    update(ui->negativeSlackColorRangeBeginLineEdit, color);
}

void PreferencesDialog::on_negativeSlackColorRangeEndPickColorButton_clicked()
{
    QColor current;
    current.setNamedColor(ui->negativeSlackColorRangeEndLineEdit->text());
    QColor color = QColorDialog::getColor(current);
    update(ui->negativeSlackColorRangeEndLineEdit, color);
}

void PreferencesDialog::on_zeroSlackColorPickColorButton_clicked()
{
    QColor current;
    current.setNamedColor(ui->zeroSlackColorLineEdit->text());
    QColor color = QColorDialog::getColor(current);
    update(ui->zeroSlackColorLineEdit, color);
}

void PreferencesDialog::on_positiveSlackColorPickColorButton_clicked()
{
    QColor current;
    current.setNamedColor(ui->positiveSlackColorLineEdit->text());
    QColor color = QColorDialog::getColor(current);
    update(ui->positiveSlackColorLineEdit, color);
}

void PreferencesDialog::accept()
{
    m_negativeSlackBegin.setNamedColor(ui->negativeSlackColorRangeBeginLineEdit->text());
    m_negativeSlackEnd.setNamedColor(ui->negativeSlackColorRangeEndLineEdit->text());
    m_zeroSlack.setNamedColor(ui->zeroSlackColorLineEdit->text());
    m_positiveSlack.setNamedColor(ui->positiveSlackColorLineEdit->text());
    QDialog::accept();
}

}
