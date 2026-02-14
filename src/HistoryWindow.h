#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QLineEdit>

class HistoryWindow : public QWidget {
    Q_OBJECT

public:
    explicit HistoryWindow(QWidget *parent = nullptr);
    void updateList(const QList<QString>& history);

signals:
    void itemSelected(const QString& text);

protected:
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onItemClicked(QListWidgetItem *item);
    void onSearchTextChanged(const QString &text);
    void toggleVisibility();

private:
    QListWidget *listWidget;
    QLineEdit *searchBox;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QLabel *headerLabel;
    QList<QString> currentHistory;
    
    void setupUI();
    void setupTray();
    void refreshView();
};

#endif // HISTORYWINDOW_H
