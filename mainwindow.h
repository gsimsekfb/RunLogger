#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTreeWidget;
class QTableWidget;
class QLabel;
class QTextEdit;
class QTreeWidgetItem;
class QPushButton;

class CreateRunLogDialog;

#include <map>
#include <functional>

#include <QEvent>
#include <QGestureEvent>
#include <QSwipeGesture>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool gestureEvent(QGestureEvent *event)
    {
        if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
            swipeTriggered(static_cast<QSwipeGesture *>(swipe));

        return true;
    }

    void swipeTriggered(QSwipeGesture *gesture)
    {
        if (gesture->state() == Qt::GestureFinished) {
            if (gesture->horizontalDirection() == QSwipeGesture::Left)
                on_leftArrowClicked();
            else if(gesture->horizontalDirection() == QSwipeGesture::Right)
                on_rightArrowClicked();
        }
    }

protected:
    void closeEvent(QCloseEvent *event) override;

    bool event(QEvent *event) override
    {
        if (event->type() == QEvent::Gesture)
            return gestureEvent(static_cast<QGestureEvent*>(event));
        return QWidget::event(event);

//        if (event->type() == QEvent::Gesture) {
//            if(auto gesture = static_cast<QGestureEvent*>(event)->gesture(Qt::SwipeGesture)){
//                if(auto swipeGesture = static_cast<QSwipeGesture*>(gesture)) {
//                    if (swipeGesture->state() == Qt::GestureFinished) {
//                        if (swipeGesture->horizontalDirection() == QSwipeGesture::Left)
//                            on_leftArrowClicked();
//                        else if (swipeGesture->horizontalDirection() == QSwipeGesture::Right)
//                            on_rightArrowClicked();

//                    }
//                }

//            }
//        }
//        return QWidget::event(event);
    }

private slots:    
    void on_addButtonClicked();
    void on_editButtonClicked();
    void on_delButtonClicked();
    void on_logsButtonClicked();
    void on_rightArrowClicked();    
    void on_leftArrowClicked();
    void backupDataFile();

    void on_itemActivated(QTreeWidgetItem *item, int column);    

private:
    void saveDataFromCreateDialog(CreateRunLogDialog const* createDialog);
    void loadMonthRunLogs(int month, int year);
    void saveAllDataMapIntoFile();
    void doBackupIfNeeded();

    QLabel* _monthAndYear;
    QTreeWidget* _runList;
    QLabel* _logsLabel;
    QTextEdit* _logs;
    QPushButton* _backupButton;
    QPushButton* _delButton;
    QPushButton* _editButton;

    //std::vector<QVector<QString>> _allData;

    // Active month's run logs
    std::map<long long, QVector<QString>, std::greater<long long>> _monthLogs;

    // Use timestamp as key: i.e. '20170803142030' which is 3-Aug-2017, 14:20:30
    // Example element of map:
    // { 20170514002959, {14, 5, 2017, 0, 29, 59, 400, 0, 0, 0, Normal} }
    std::map<long long, QString, std::greater<long long>> _allDataMap;

};

#endif // MAINWINDOW_H
