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

    // Set flag so we don't re-add this immediately to history as a "new" copy event logic if we want specialized handling
    // But generally, moving it to top is desired, so maybe we just let onClipboardChanged handle it?
    // Let's explicitly move it to top here to be responsive, and handle the duplicate check in onClipboardChanged.
    
    isSelfUpdate = true;
    clipboard->setText(text);
    isSelfUpdate = false;

    // Move to top locally as well for instant UI update
    history.removeAll(text);
    history.prepend(text);
    emit historyChanged(history);
}

void ClipboardManager::onClipboardChanged() {
    // If we just set the clipboard ourselves, we might want to skip logic or just confirm it's there.
    if (isSelfUpdate) return; 

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
