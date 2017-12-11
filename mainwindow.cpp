
#include "mainwindow.h"

#include "createrunlogdialog.h"

#include <QPushButton>
#include <QGridLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTreeWidget>
#include <QTableWidget>
#include <QLabel>
#include <QDate>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QTextEdit>
#include <QScrollBar>
#include <QScroller>
#include <QTimer>


// Todo:
//  - qt quick for native look
//  + replace allData with allDataMap
//  + update _monthsLoads after addactivity


struct RunLogItem : public QTreeWidgetItem
{
    RunLogItem(long long id)
        : id(id) {}

    long long const id = 0;
};

bool fileExists(const QString& path);
bool fileExists(const QFile& file);

QString const HEADER_LINE = "day, mon, year, hour, min, sec, "
                            "distance_in_meter, dur_min, dur_sec, dur_msec, speed, notes";

QString const DATA_FILE_WIN = "C:\\Users\\gsimsek\\Desktop\\run_log_app\\runlog.csv";
QString const DATA_FILE_AND = "/sdcard/Android/data/a.runlogger/runlog.csv";
QString const DATA_DIR_AND = "/sdcard/Android/data/a.runlogger/";


QString const BACKUP_TIMESTAMP_FILE_WIN =
        "C:\\Users\\gsimsek\\Desktop\\run_log_app\\lastBackupTimestamp.txt";

QString const BACKUP_TIMESTAMP_FILE_AND =
        "/sdcard/Android/data/a.runlogger/lastBackupTimestamp.txt";

QString const DATE_FORMAT = "dd.MM.yyyy";


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) //, ui(new Ui::MainWindow)
{    
    auto leftArrow = new QPushButton;
    leftArrow->setIcon(QIcon(":/icons/left_arrow.png"));
    connect(leftArrow, &QPushButton::clicked, this, &MainWindow::on_leftArrowClicked);

    QString const currentMonth = QDate::shortMonthName(QDate::currentDate().month());
    QString const currentYear = QString::number(QDate::currentDate().year());
    _monthAndYear = new QLabel(currentMonth + ' ' + currentYear);
    //_monthAndYear->setStyleSheet("font-weight: bold;");

    auto rightArrow = new QPushButton;
    rightArrow->setIcon(QIcon(":/icons/right_arrow.png"));
    connect(rightArrow, &QPushButton::clicked, this, &MainWindow::on_rightArrowClicked);

    _runList = new QTreeWidget;
    _runList->setHeaderHidden(true);
    _runList->setIndentation(0);
    _runList->setStyleSheet( "QTreeView::item {height: 80px;}");
    //_runList->verticalScrollBar()->setHidden(true);
    //_runList->verticalScrollBar()->setFixedWidth(10);
    _runList->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 40px;}");

    QScroller::grabGesture(_runList->viewport(), QScroller::LeftMouseButtonGesture);

    connect(_runList, &QTreeWidget::itemClicked, this, &MainWindow::on_itemActivated);

    _logsLabel = new QLabel("\nDebug Logs:");
    _logsLabel->setVisible(false);
    _logs = new QTextEdit("Info: Program started at " + QTime::currentTime().toString() + ", "
                          + QDate::currentDate().toString() + '.');
    _logs->setVisible(false);

#ifdef _WIN32
    QString filePath = DATA_FILE_WIN;
    QFile file(DATA_FILE_WIN);
#elif __ANDROID__
    if (!QDir(DATA_DIR_AND).exists())
        QDir().mkdir(DATA_DIR_AND);

    QString filePath = DATA_FILE_AND;
    QFile file(DATA_FILE_AND);
#endif

    if (!fileExists(filePath)) {
        if(file.open(QIODevice::WriteOnly)) {
           QTextStream stream(&file);
           stream << HEADER_LINE << endl;
        }
    }
    else {  // Data file exists
        // Load all run logs as values into data map (to sort)
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream textStream(&file);
            while (!textStream.atEnd()) {
                QString line = textStream.readLine();
                auto const& lineVec =  line.split(", ").toVector();
                if(lineVec.size() < 10) {
                    _logs->append("Error: MainWindow::MainWindow(): lineVec size is smaller than 10.");
                    break;
                }
                QString const year = lineVec[2];
                QString const month = lineVec[1].toInt() < 10 ? '0' + lineVec[1] : lineVec[1];
                QString const day = lineVec[0].toInt() < 10 ? '0' + lineVec[0] : lineVec[0];
                QString const hour = lineVec[3].toInt() < 10 ? '0' + lineVec[3] : lineVec[3];
                QString const min = lineVec[4].toInt() < 10 ? '0' + lineVec[4] : lineVec[4];
                QString const sec = lineVec[5].toInt() < 10 ? '0' + lineVec[5] : lineVec[5];

                if(lineVec.size() == 10) // add speed and notes
                    line.append(", Null, Null");

                if(lineVec.size() == 11) // add notes
                    line.append(", Null");

                _allDataMap.insert({(year + month + day + hour + min + sec).toLongLong(), line});
            }
            file.close();
        }
        _allDataMap.erase(std::prev(_allDataMap.end()));

        // Load current month's run logs
        loadMonthRunLogs(QDate::currentDate().month(), QDate::currentDate().year());
    }

    _backupButton = new QPushButton("Backup");
    _backupButton->setVisible(false);
    connect(_backupButton, &QPushButton::clicked, this, &MainWindow::backupDataFile);

    auto logsButton = new QPushButton;
    logsButton->setIcon(QIcon(":/icons/log.png"));
    connect(logsButton, &QPushButton::clicked, this, &MainWindow::on_logsButtonClicked);

    _delButton = new QPushButton;
    _delButton->setIcon(QIcon(":/icons/minus.png"));
    connect(_delButton, &QPushButton::clicked, this, &MainWindow::on_delButtonClicked);

    auto addButton = new QPushButton;
    addButton->setIcon(QIcon(":/icons/plus.png"));
    connect(addButton, &QPushButton::clicked, this, &MainWindow::on_addButtonClicked);

    _editButton = new QPushButton;
    _editButton->setIcon(QIcon(":/icons/edit.png"));
    connect(_editButton, &QPushButton::clicked, this, &MainWindow::on_editButtonClicked);

    auto buttonsLay = new QHBoxLayout;
    buttonsLay->addWidget(logsButton);
    buttonsLay->addWidget(_delButton);
    buttonsLay->addWidget(_editButton);
    buttonsLay->addWidget(addButton);
    buttonsLay->addSpacing(40);
    buttonsLay->addWidget(leftArrow);
    buttonsLay->addWidget(rightArrow);

    auto lay = new QGridLayout;
    //lay->addWidget(leftArrow,       0, 0);
    lay->addWidget(_monthAndYear,   0, 1, 1, 2, Qt::AlignCenter);
    //lay->addWidget(rightArrow,      0, 3);
    lay->addWidget(_runList,        1, 0, 1, 4);
    //lay->addWidget(new QLabel(),                2, 0, 1, 4);
    lay->addWidget(_logsLabel,      3, 0);
    lay->addWidget(_logs,           4, 0, 1, 4);
    lay->addWidget(_backupButton,   5, 0, 1, 1);
    lay->addLayout(buttonsLay,      6, 0, 1, 4);

    auto centralWid = new QWidget;
    centralWid->setLayout(lay);
    setCentralWidget(centralWid);

#ifdef _WIN32
    setFixedSize(1080, 1920);
#endif

//#ifdef __ANDROID__
//    auto minH = leftArrow->sizeHint().height();
//    setStyleSheet("QPushButton {  min-height:" + QString::number(2.5*minH) + " px; }");
//#endif

    setWindowIcon(QIcon(":/icons/run_man_icon.png"));
    grabGesture(Qt::GestureType::SwipeGesture);

//    _runList->verticalScrollBar()->setFixedWidth(200);
//    _backupButton->setFixedWidth(_backupButton->sizeHint().width()*1.6);

}


MainWindow::~MainWindow()
{

}

//bool MainWindow::gestureEvent(QGestureEvent *event)
//{

//}

void MainWindow::closeEvent(QCloseEvent *event)
{    
    saveAllDataMapIntoFile();

    QMainWindow::closeEvent(event);
}

void MainWindow::on_addButtonClicked()
{
    // Extract year and month from top _monthAndYear widget as int
    int pageYearInt = _monthAndYear->text().split(' ')[1].toInt();
    auto pageMonthStr = _monthAndYear->text().split(' ')[0];
    int pageMonthInt = 0;
    for(int i = 1; i < 13; ++i) {
        if(QDate::shortMonthName(i) == pageMonthStr) {
            pageMonthInt = i;
            break;
        }
    }

    // Prompt add activity dialog and get required data from it
    auto createDialog = new CreateRunLogDialog(pageYearInt, pageMonthInt);
    if(!createDialog->exec())
        return;

    saveDataFromCreateDialog(createDialog);
    backupDataFile();
}

void MainWindow::on_editButtonClicked()
{
    if(auto const selectedItem = dynamic_cast<RunLogItem*>(_runList->currentItem())) {
        auto const id = selectedItem->id;
        auto const& runLog = _monthLogs.find(id)->second;

        // Prompt add activity dialog and get required data from it
        auto createDialog = new CreateRunLogDialog(runLog[2].toInt(), runLog[1].toInt(),
                runLog[0].toInt(), runLog[3].toInt(), runLog[4].toInt(), runLog[5].toInt(),
                runLog[6].toInt(), runLog[7].toInt(), runLog[8].toInt(), runLog[9].toInt(),
                (runLog[10] == "Sprint"), runLog[11]);
        if(!createDialog->exec())
            return;

        //delete selectedItem;
        _monthLogs.erase(id);
        _allDataMap.erase(id);
        saveDataFromCreateDialog(createDialog);
        backupDataFile();
    }
}

void MainWindow::on_delButtonClicked()
{
    for(auto const selectedItem : _runList->selectedItems()) {
        if(auto runLogItem = dynamic_cast<RunLogItem*>(selectedItem)){
            _monthLogs.erase(runLogItem->id);
            _allDataMap.erase(runLogItem->id);
            saveAllDataMapIntoFile();
            backupDataFile();
            delete selectedItem;
        }
    }
}

void MainWindow::backupDataFile()
{
#ifdef _WIN32
    QString const& dataFile = DATA_FILE_WIN;
//    QFile backupTimestampFile(BACKUP_TIMESTAMP_FILE_WIN);
#elif __ANDROID__
    QString const& dataFile = DATA_FILE_AND;
//    QFile backupTimestampFile(BACKUP_TIMESTAMP_FILE_AND);
#endif

    QString const todayStr = QDate::currentDate().toString(DATE_FORMAT);

    //// Currently backupTimestampFile is not used
//    if (backupTimestampFile.open(QIODevice::WriteOnly)) {
//           QTextStream stream(&backupTimestampFile);
//           stream << todayStr << endl;
//    }

    QString const& backupFile = dataFile + '.' + todayStr;
    QFile::copy(dataFile, backupFile);
    if(fileExists(backupFile))
        _logs->append("Info: Data file backed up as " + backupFile + '.');
    else
        _logs->append("Error: Failed to backup data file.");
}

void MainWindow::on_logsButtonClicked()
{
    _logsLabel->setVisible(!_logsLabel->isVisible());
    _logs->setVisible(!_logs->isVisible());
    _backupButton->setVisible(!_backupButton->isVisible());
}

void MainWindow::on_rightArrowClicked()
{
    auto month = _monthAndYear->text().split(' ')[0];
    auto year = _monthAndYear->text().split(' ')[1];

    int monthInt = 0;
    for(int i = 1; i < 13; ++i) {
        if(QDate::shortMonthName(i) == month) {
            monthInt = i;
            break;
        }
    }
    ++monthInt;

    if(monthInt > 12) {
        year = QString::number(year.toInt() + 1);
        monthInt %= 12;
    }

    _monthAndYear->setText(QDate::shortMonthName(monthInt) + ' ' + year);

    // Load month logs
    loadMonthRunLogs(monthInt, year.toInt());
}

void MainWindow::on_leftArrowClicked()
{
    auto month = _monthAndYear->text().split(' ')[0];
    auto year = _monthAndYear->text().split(' ')[1];

    int monthInt = 0;
    for(int i = 1; i < 13; ++i) {
        if(QDate::shortMonthName(i) == month) {
            monthInt = i;
            break;
        }
    }
    --monthInt;

    if(0 == monthInt) {
        year = QString::number(year.toInt() - 1);
        monthInt = 12;
    }

    _monthAndYear->setText(QDate::shortMonthName(monthInt) + ' ' + year);

    // Load month logs
    loadMonthRunLogs(monthInt, year.toInt());
}

void MainWindow::on_itemActivated(QTreeWidgetItem *item, int column)
{
    //bool isRunLogItem = dynamic_cast<RunLogItem*>(item);
    //_delButton->setEnabled(isRunLogItem);
    //_editButton->setEnabled(isRunLogItem);
}

void MainWindow::saveDataFromCreateDialog(CreateRunLogDialog const* createDialog)
{
    QDate const& date = createDialog->date();
    QTime const time = createDialog->time();
    Duration const& duration = createDialog->getDuration();
    int const distance = createDialog->getDistanceInMeters();
    QString const& speed = createDialog->getSpeed();
    QString const& notes = createDialog->getNotes();

    // Insert the new run log into month data and all data containers
    QString const lline {
        QString::number(date.day()) + ", " + QString::number(date.month())
                + ", " + QString::number(date.year()) + ", " + QString::number(time.hour())
                + ", " + QString::number(time.minute()) + ", " + QString::number(time.second())
                + ", " + QString::number(distance)
                + ", " + QString::number(duration.min) + ", " + QString::number(duration.sec)
                + ", " + QString::number(duration.msec) + ", " + speed + ", " + notes
    };
    QString const& yyear = QString::number(date.year());
    QString const& mmonth = date.month() < 10 ? '0' + QString::number(date.month())
                                              : QString::number(date.month());
    QString const& dday = date.day() < 10 ? '0' + QString::number(date.day())
                                          : QString::number(date.day());
    QString const& hhour = time.hour() < 10 ? '0' + QString::number(time.hour())
                                            : QString::number(time.hour());
    QString const& mmin = time.minute() < 10 ? '0' + QString::number(time.minute())
                                             : QString::number(time.minute());
    QString const& ssec = time.second() < 10 ? '0' + QString::number(time.second())
                                             : QString::number(time.second());
    _allDataMap.insert({(yyear + mmonth + dday + hhour + mmin + ssec).toLongLong(), lline});
    auto const& lineVec =  lline.split(", ").toVector();
    _monthLogs.insert({(yyear + mmonth + dday + hhour + mmin + ssec).toLongLong(), lineVec});

    //// Load run logs for the month of new log
    _monthAndYear->setText(QDate::shortMonthName(date.month()) + ' '
                           + QString::number(date.year()));

    loadMonthRunLogs(date.month(), date.year());
    saveAllDataMapIntoFile();
}
void MainWindow::loadMonthRunLogs(int month, int year)
{
    // Extract month logs from all data map
    _monthLogs.clear();
    for(auto const& keyAndLine : _allDataMap) {
        auto const& line = keyAndLine.second;
        auto const& lineVec =  line.split(", ").toVector();
        if(lineVec.size() < 3) {
            _logs->append("Error: MainWindow::loadMonthRunLogs(): lineVec size is smaller than 3.");
            break;
        }
        if(lineVec[1].toInt() == month && lineVec[2].toInt() ==  year)
            _monthLogs.insert({keyAndLine.first, lineVec});
    }

    // Reload run log list UI
    _runList->clear();
    int lastWeekNoAdded = 0;
    int lastDayOfMonthAdded = 0;
    for(auto const& idAndline : _monthLogs) {
        auto const& line = idAndline.second;
        if(line.size() < 10) {
            _logs->append("Error: MainWindow::loadMonthRunLogs(): lineVec size is smaller than 11.");
            break;
        }

        auto date = QDate(line[2].toInt(), line[1].toInt(), line[0].toInt());

        auto const weekNo = date.weekNumber();
        if(weekNo != lastWeekNoAdded) {
            auto weekNoItem = new QTreeWidgetItem;
            weekNoItem->setBackgroundColor(0,"#999999");
            weekNoItem->setText(0, "Week " + QString::number(weekNo));
            _runList->addTopLevelItem(weekNoItem);
            lastWeekNoAdded = weekNo;
        }

        if(lastDayOfMonthAdded != line[0].toInt()) {
            auto dayOfMonthItem = new QTreeWidgetItem;
            dayOfMonthItem->setBackgroundColor(0,"#e6e6e6");
            dayOfMonthItem->setText(0, line[0] + ' ' +  QDate::shortMonthName(line[1].toInt())
                    + ", " + QDate::shortDayName(date.dayOfWeek()));
            _runList->addTopLevelItem(dayOfMonthItem);
            lastDayOfMonthAdded = line[0].toInt();
        }

        auto durMin = (line[7].toInt() < 10) ? '0' + line[7] : line[7] ;
        auto durSec = (line[8].toInt() < 10) ? '0' + line[8] : line[8] ;
        QString dur = durMin + ':' + durSec + "." + line[9];

        auto item = new RunLogItem(idAndline.first);
        auto distance = line[6].toInt() < 1000 ? (line[6] + " m") :
            (QString::number(line[6].toDouble()/1000) + " km");

        item->setIcon(0, QIcon(":/icons/run_man_icon.png"));        
        item->setText(0, dur + "  -  " + distance);

        if(line.size() > 10 && line[10] == "Sprint")   // if there is 'speed'
            item->setText(0, item->text(0) + " (" + line[10] + ")");

        if(line[11] != "Null")   // if there is 'notes'
            item->setText(0, item->text(0) + " (" + line[11] + ")");

        //item->setFlags(item->flags() | Qt::ItemIsEditable);
        _runList->addTopLevelItem(item);
    }
}

void MainWindow::saveAllDataMapIntoFile()
{
#ifdef _WIN32
    QFile file(DATA_FILE_WIN);
#elif __ANDROID__
    QFile file(DATA_FILE_AND);
#endif

    // Save run log into data file
    if (file.open(QIODevice::WriteOnly /*| QIODevice::Append*/)) {
           QTextStream stream(&file);
           stream << HEADER_LINE << endl;
           for(auto const& keyAndline : _allDataMap)
               stream << keyAndline.second << endl;
    }
}

// Currently not used
// Perform backup in two conditions:
// 1. backupTimestampFile does not exist
// 2. Last backup timestamp is more than 3 days old
void MainWindow::doBackupIfNeeded()
{
#ifdef _WIN32
    QFile backupTimestampFile(BACKUP_TIMESTAMP_FILE_WIN);
#elif __ANDROID__
    QFile backupTimestampFile(BACKUP_TIMESTAMP_FILE_AND);
#endif

    if(!fileExists(backupTimestampFile)) {
        backupDataFile();
        return;
    }

    if (backupTimestampFile.open(QIODevice::ReadOnly)) {
        QTextStream textStream(&backupTimestampFile);
        QString line;

        while (!textStream.atEnd())
            line = textStream.readLine();

        if(!line.isEmpty()) {
            auto const lastBackupDateTime = QDateTime::fromString(line, DATE_FORMAT);
            if(lastBackupDateTime.isValid()) {
                auto const now = QDateTime::currentDateTime();

                if(lastBackupDateTime.daysTo(now) > 3)
                    backupDataFile();
            }
        }
    }

}

// ------- End of class MainWindow

bool fileExists(const QString& path) {
    QFileInfo const fileInfo(path);
    return fileInfo.exists() && fileInfo.isFile();
}

bool fileExists(const QFile& file) {
    QFileInfo const fileInfo(file);
    return fileInfo.exists() && fileInfo.isFile();
}




