#include <QApplication>
#include "ClipboardManager.h"
#include "HistoryWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Prevent closing the app when the last window (history window) is closed
    app.setQuitOnLastWindowClosed(false);

    ClipboardManager clipboardManager;
    HistoryWindow historyWindow;

    // Connect logic to UI
    QObject::connect(&clipboardManager, &ClipboardManager::historyChanged, 
                     &historyWindow, &HistoryWindow::updateList);

    QObject::connect(&historyWindow, &HistoryWindow::itemSelected,
                     &clipboardManager, &ClipboardManager::copyToClipboard);

    return app.exec();
}
