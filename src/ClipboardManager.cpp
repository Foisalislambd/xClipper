#include "ClipboardManager.h"
#include <QMimeData>
#include <QDebug>

ClipboardManager::ClipboardManager(QObject *parent) : QObject(parent) {
    clipboard = QApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &ClipboardManager::onClipboardChanged);
}

const QList<QString>& ClipboardManager::getHistory() const {
    return history;
}

void ClipboardManager::copyToClipboard(const QString& text) {
    if (text.isEmpty()) return;
    clipboard->setText(text);
}

void ClipboardManager::onClipboardChanged() {
    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData && mimeData->hasText()) {
        QString text = mimeData->text();
        if (text.trimmed().isEmpty()) return;

        // Avoid duplicates at the top
        if (!history.isEmpty() && history.first() == text) {
            return;
        }

        // If exists elsewhere, remove it so it moves to top
        if (history.contains(text)) {
            history.removeAll(text);
        }

        history.prepend(text);

        if (history.size() > MAX_HISTORY) {
            history.removeLast();
        }

        emit historyChanged(history);
    }
}
