#ifndef CREATERUNLOGDIALOG_H
#define CREATERUNLOGDIALOG_H

#include <QDialog>

class QLineEdit;
class QDate;
class QDateEdit;
class QTimeEdit;
class QCheckBox;
class QCalendarWidget;

struct Duration
{
    Duration(int min, int sec, int msec)
        : min(min), sec(sec), msec(msec) {}

    int const min, sec, msec;
};

class CreateRunLogDialog : public QDialog
{
    //enum TYPE { CREATE_DIALOG, EDIT_DIALOG };

public:
    // Create log
    CreateRunLogDialog(int year, int month);

    // Edit log
    CreateRunLogDialog(int year, int month, int day, int hour, int min, int sec, int distance,
                       int duration_min, int duration_sec, int duraion_msec, bool isSprint,
                       QString const& notes);

    void init(bool isCreateDialog);    // work as ctor

    QDate date() const;
    QTime time() const;
    Duration getDuration() const;
    int getDistanceInMeters() const;
    double getDistanceInKm() const;
    QString getSpeed() const;
    QString getNotes() const;

public slots:
    void accept() override;

private slots:
    void on_inputChange();
    void on_clearButtonClicked();
    void resize_to_content();

private:

    QDateEdit* _dateEdit;
    //QCalendarWidget* _dateEdit;
    QTimeEdit* _timeEdit;

    QLineEdit* _kmEdit;
    QLineEdit* _mEdit;
    QLineEdit* _notes;

    QCheckBox* _sprintCheckBox;

    QLineEdit* _startMin;
    QLineEdit* _startSec;
    QLineEdit* _startMill;
    QLineEdit* _endMin;
    QLineEdit* _endSec;
    QLineEdit* _endMill;
    QLineEdit* _diffMin;
    QLineEdit* _diffSec;
    QLineEdit* _diffMill;

    std::vector<int> _ctorParams;
    bool const _isSprint = false;
};

#endif // CREATERUNLOGDIALOG_H
