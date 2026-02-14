#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include <QObject>
#include <QClipboard>
#include <QList>
#include <QString>
#include <QApplication>

class ClipboardManager : public QObject {
    Q_OBJECT

public:
    explicit ClipboardManager(QObject *parent = nullptr);
    const QList<QString>& getHistory() const;

signals:
    void historyChanged(const QList<QString>& history);

public slots:
    void copyToClipboard(const QString& text);
    void onClipboardChanged();

private:
    QClipboard *clipboard;
    QList<QString> history;
    const int MAX_HISTORY = 20;
    bool isSelfUpdate = false; // Flag to ignore updates triggered by ourselves
};

#endif // CLIPBOARDMANAGER_H
