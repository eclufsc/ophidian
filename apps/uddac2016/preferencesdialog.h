#ifndef OPHIDIAN_APPS_UDDAC2016_PREFERENCESDIALOG_H
#define OPHIDIAN_APPS_UDDAC2016_PREFERENCESDIALOG_H

#include <QDialog>

class QLineEdit;

namespace uddac2016 {

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

    QColor m_negativeSlackBegin;
    QColor m_negativeSlackEnd;
    QColor m_zeroSlack;
    QColor m_positiveSlack;
    QColor m_noSlack;


    void update(QLineEdit* lineedit, const QColor & color);

public:
    void draw();
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();


    const QColor & negativeSlackBegin() const {
        return m_negativeSlackBegin;
    }

    const QColor & negativeSlackEnd() const {
        return m_negativeSlackEnd;
    }

    const QColor & zeroSlack() const {
        return m_zeroSlack;
    }

    const QColor & positiveSlack() const {
        return m_positiveSlack;
    }

private slots:
    void on_negativeSlackColorRangeBeginPickColorButton_clicked();
    void on_negativeSlackColorRangeEndPickColorButton_clicked();
    void on_zeroSlackColorPickColorButton_clicked();
    void on_positiveSlackColorPickColorButton_clicked();
    void accept();

private:
    Ui::PreferencesDialog *ui;
};

}

#endif // OPHIDIAN_APPS_UDDAC2016_PREFERENCESDIALOG_H
