#include "createrunlogdialog.h"

#include <QPushButton>
#include <QGridLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTreeWidget>
#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>
#include <QSpacerItem>
#include <QCheckBox>

//namespace CreateRunLogDialog
//{
//int xx= 9;
//    //enum { CREATE, EDIT };
//}


CreateRunLogDialog::CreateRunLogDialog(int year, int month)
    : _ctorParams({year, month})
{    
    setWindowTitle("Create Run Log");
    init(true);
}

CreateRunLogDialog::CreateRunLogDialog(int year, int month, int day, int hour, int min, int sec,
                   int distance, int duration_min, int duration_sec, int duraion_msec, bool isSprint,
                   QString const& notes)
    : _ctorParams({year, month, day, hour, min, sec, distance, duration_min, duration_sec,
                  duraion_msec}),
      _isSprint(isSprint)
{
    setWindowTitle("Edit Run Log");
    init(false);
    if(notes != "Null")
        _notes->setText(notes);
}

void CreateRunLogDialog::init(bool isCreateDialog)
{
    _dateEdit = new QDateEdit;
    _dateEdit->setCalendarPopup(true);

    if(isCreateDialog) {
        int year = _ctorParams[0], month = _ctorParams[1];
        if(QDate::currentDate().year() == year && QDate::currentDate().month() == month)
            _dateEdit->setDate(QDate(year, month, QDate::currentDate().day()));
        else
            _dateEdit->setDate(QDate(year, month, 1));
    }
    else // edit dialog
        _dateEdit->setDate(QDate(_ctorParams[0], _ctorParams[1], _ctorParams[2]));

    _timeEdit = new QTimeEdit;
    _timeEdit->setTime(isCreateDialog ? QTime::currentTime()
                                      : QTime(_ctorParams[3], _ctorParams[4], _ctorParams[5]));

    _kmEdit = new QLineEdit(isCreateDialog ? "" : QString::number(_ctorParams[6]/1000));
    _kmEdit->setInputMethodHints(Qt::ImhPreferNumbers);
    _mEdit = new QLineEdit(isCreateDialog ? "" : QString::number(_ctorParams[6] % 1000));
    _mEdit->setInputMethodHints(Qt::ImhPreferNumbers);

    _sprintCheckBox = new QCheckBox("Sprint");
    _sprintCheckBox->setChecked(isCreateDialog ? false : _isSprint);
    auto kmEditHeight = QString::number(_kmEdit->sizeHint().height()*0.7);
    _sprintCheckBox->setStyleSheet(
                "QCheckBox::indicator { width:" + kmEditHeight + "px; " +
                                       "height: " + kmEditHeight + "px;}");

    _notes = new QLineEdit;

    _startMin = new QLineEdit;
    _startSec = new QLineEdit;
    _startMill = new QLineEdit;

    _endMin = new QLineEdit;
    _endSec = new QLineEdit;
    _endMill = new QLineEdit;

    _diffMin = new QLineEdit(isCreateDialog ? "" : QString::number(_ctorParams[7]));
    _diffSec = new QLineEdit(isCreateDialog ? "" : QString::number(_ctorParams[8]));
    _diffMill = new QLineEdit(isCreateDialog ? "" : QString::number(_ctorParams[9]));

    _startMin ->setInputMethodHints(Qt::ImhPreferNumbers);
    _startSec ->setInputMethodHints(Qt::ImhPreferNumbers);
    _startMill->setInputMethodHints(Qt::ImhPreferNumbers);
    _endMin->setInputMethodHints(Qt::ImhPreferNumbers);
    _endSec->setInputMethodHints(Qt::ImhPreferNumbers);
    _endMill->setInputMethodHints(Qt::ImhPreferNumbers);
    _diffMin->setInputMethodHints(Qt::ImhPreferNumbers);
    _diffSec->setInputMethodHints(Qt::ImhPreferNumbers);
    _diffMill->setInputMethodHints(Qt::ImhPreferNumbers);

    auto clearButton = new QPushButton("Clear");
    connect(clearButton, &QPushButton::clicked, this, &CreateRunLogDialog::on_clearButtonClicked);
    clearButton->setAutoDefault(false);

    auto cancelButton = new QPushButton("Cancel");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    cancelButton->setAutoDefault(false);

    auto saveButton = new QPushButton("Save");
    connect(saveButton, &QPushButton::clicked, this, &QDialog::accept);
    saveButton->setAutoDefault(true);

    // Main layout
    auto mainLay = new QGridLayout;
    mainLay->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    auto subGridLay = new QGridLayout;
    subGridLay->setVerticalSpacing(25);
    subGridLay->setHorizontalSpacing(20);
    auto verLine = new QFrame;
    verLine->setFrameShape(QFrame::VLine);
    verLine->setFrameShadow(QFrame::Sunken);

    subGridLay->addWidget(new QLabel("Date:"),         1, 0);
    auto subHLay = new QHBoxLayout;
    subHLay->setAlignment(Qt::AlignLeft);
    subHLay->setSpacing(20);
    subHLay->addWidget(_dateEdit);
    subHLay->addWidget(_timeEdit);
    //subHLay->addStretch();
    subGridLay->addLayout(subHLay, 1, 1);

    auto disHLay = new QHBoxLayout;
    disHLay->setSpacing(20);
    disHLay->setAlignment(Qt::AlignLeft);
    disHLay->addWidget(_kmEdit);
    disHLay->addWidget(new QLabel("km, "));
    disHLay->addWidget(_mEdit);
    disHLay->addWidget(new QLabel("m"));
    disHLay->addSpacing(20);
    disHLay->addWidget(verLine);
    disHLay->addWidget(_sprintCheckBox);
    //disHLay->addStretch();

    auto speedHLay = new QHBoxLayout;
    speedHLay->setSpacing(20);
    //speedHLay->addWidget(_sprintCheckBox);
    //speedHLay->addSpacing(20);
    //speedHLay->addWidget(verLine);
    //speedHLay->addSpacing(20);
    speedHLay->addWidget(new QLabel("Notes:"));
    speedHLay->addWidget(_notes, 1);

    subGridLay->addWidget(new QLabel("Distance:"),     2, 0);
    subGridLay->addLayout(disHLay,                     2, 1);
    //subGridLay->addWidget(new QLabel("Speed:"),        3, 0);
    subGridLay->addLayout(speedHLay,                   3, 0, 1, 2);

    mainLay->addWidget(new QLabel(" "),             4, 0);
    mainLay->addWidget(new QLabel("Min"),           5, 1, Qt::AlignCenter);
    mainLay->addWidget(new QLabel("Sec"),           5, 3, Qt::AlignCenter);
    auto msec = new QLabel("msec (x100)");
    mainLay->addWidget(msec,                        5, 5, Qt::AlignCenter);

    mainLay->addWidget(new QLabel("Start: "),       6, 0);
    mainLay->addWidget(_startMin,                   6, 1);
    mainLay->addWidget(new QLabel(":"),             6, 2);
    mainLay->addWidget(_startSec,                   6, 3);
    mainLay->addWidget(new QLabel("."),             6, 4);
    mainLay->addWidget(_startMill,                  6, 5);

    mainLay->addWidget(new QLabel("End: "),         7, 0);
    mainLay->addWidget(_endMin,                     7, 1);
    mainLay->addWidget(new QLabel(":"),             7, 2);
    mainLay->addWidget(_endSec,                     7, 3);
    mainLay->addWidget(new QLabel("."),             7, 4);
    mainLay->addWidget(_endMill,                    7, 5);

    mainLay->addWidget(new QLabel("Diff: "),        8, 0);
    mainLay->addWidget(_diffMin,                    8, 1);
    mainLay->addWidget(new QLabel(":"),             8, 2);
    mainLay->addWidget(_diffSec,                    8, 3);
    mainLay->addWidget(new QLabel("."),             8, 4);
    mainLay->addWidget(_diffMill,                   8, 5);

    mainLay->addWidget(clearButton,                 9, 1);
    mainLay->addWidget(cancelButton,                9, 3);
    mainLay->addWidget(saveButton,                  9, 5);

    //mainLay->addWidget(buttonBox,                   9, 3, 1, 3, Qt::AlignRight);

    mainLay->addWidget(new QLabel(" "),             11, 0);
    mainLay->addWidget(new QLabel(" "),             12, 0);
    mainLay->addWidget(new QLabel(" "),             13, 0);
    mainLay->addWidget(new QLabel(" "),             14, 0);
    mainLay->addWidget(new QLabel(" "),             15, 0);
    mainLay->addWidget(new QLabel(" "),             16, 0);
    mainLay->addWidget(new QLabel(" "),             17, 0);
    mainLay->addWidget(new QLabel(" "),             18, 0);
    mainLay->addWidget(new QLabel(" "),             19, 0);
    mainLay->addWidget(new QLabel(" "),             20, 0);

    mainLay->setVerticalSpacing(25);

    auto lay = new QVBoxLayout;
    lay->addStretch();
    lay->addLayout(subGridLay);
    lay->addLayout(mainLay);

    setLayout(lay);

    // connections
    connect(_startMin, &QLineEdit::textChanged, this, &CreateRunLogDialog::on_inputChange);
    connect(_endMin, &QLineEdit::textChanged, this, &CreateRunLogDialog::on_inputChange);
    connect(_startSec, &QLineEdit::textChanged, this, &CreateRunLogDialog::on_inputChange);
    connect(_endSec, &QLineEdit::textChanged, this, &CreateRunLogDialog::on_inputChange);
    connect(_startMill, &QLineEdit::textChanged, this, &CreateRunLogDialog::on_inputChange);
    connect(_endMill, &QLineEdit::textChanged, this, &CreateRunLogDialog::on_inputChange);

#ifdef __ANDROID__
    showFullScreen();
#else
    setFixedSize(1080, 1720);
#endif

    auto const tempH = _dateEdit->sizeHint().height()*1.2;
    _dateEdit->setFixedHeight(tempH);
    _timeEdit->setFixedHeight(tempH);

    _timeEdit->setMaximumWidth(_timeEdit->sizeHint().width()*2.0);
    //_dateEdit->setMinimumWidth(_dateEdit->sizeHint().width()*1.5);


//    auto minH = clearButton->sizeHint().height();
//    setStyleSheet("QPushButton {  min-height:" + QString::number(1.5*minH) + " px; }");

//    auto minH2 = _dateEdit-> sizeHint().height();
//    setStyleSheet("QPushButton {  min-height:" + QString::number(1.5*minH2) + " px; }");

//    _kmEdit->setFixedWidth(_kmEdit->sizeHint().width()*0.17);
//    _mEdit->setFixedWidth(_kmEdit->width()*1.5);

}

QDate CreateRunLogDialog::date() const
{
    return _dateEdit->date();
}

QTime CreateRunLogDialog::time() const
{
    return _timeEdit->time();
}

Duration CreateRunLogDialog::getDuration() const
{
    return Duration(_diffMin->text().toInt(), _diffSec->text().toInt(), _diffMill->text().toInt());
}

int CreateRunLogDialog::getDistanceInMeters() const
{
    return _kmEdit->text().toInt() * 1000 + _mEdit->text().toInt();
}

double CreateRunLogDialog::getDistanceInKm() const
{
    return _kmEdit->text().toDouble() + _mEdit->text().toDouble()/1000;
}

QString CreateRunLogDialog::getSpeed() const
{
    return (_sprintCheckBox->isChecked() ? "Sprint" : "Null");
}

QString CreateRunLogDialog::getNotes() const
{
    return (_notes->text().isEmpty() ? "Null" : _notes->text());
}

void CreateRunLogDialog::accept()
{
    QDialog::accept();
}

void CreateRunLogDialog::on_inputChange()
{
    if(_endMin->text().isEmpty() || _startMin->text().isEmpty() ||
       _endSec->text().isEmpty() || _startSec->text().isEmpty() ||
       _endMill->text().isEmpty() || _startMill->text().isEmpty())
        return;

    // convert everything to msec, calculate difference in msec as double.
    int endSum = _endMin->text().toInt() * 60 * 1000 + _endSec->text().toInt() * 1000
                  + _endMill->text().toInt()*100;
    int startSum = _startMin->text().toInt() * 60 * 1000 + _startSec->text().toInt() * 1000
                  + _startMill->text().toInt()*100;
    int diffSum = endSum - startSum;

    _diffMin->setText(QString::number(diffSum / 60000));
    _diffSec->setText(QString::number((diffSum % 60000)/1000));
    _diffMill->setText(QString::number((diffSum % 1000)/100));
}

void CreateRunLogDialog::on_clearButtonClicked()
{
    _startMin->clear();
    _endMin->clear();
    _startSec->clear();
    _endSec->clear();
    _startMill->clear();
    _endMill->clear();
    _diffMin->clear();
    _diffSec->clear();
    _diffMill->clear();
}

void CreateRunLogDialog::resize_to_content()
{
    QString const& text = _notes->text();

    //use QFontMetrics this way;
    QFont font("", 0);
    QFontMetrics fm(font);
    int pixelsWide = fm.width(text);

    if(pixelsWide > _notes->width())
        _notes->setFixedWidth(pixelsWide);

    adjustSize();
}

