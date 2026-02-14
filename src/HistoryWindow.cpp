#include "HistoryWindow.h"
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <QStyle>
#include <QCursor>
#include <QFocusEvent>
#include <QRegularExpression>

HistoryWindow::HistoryWindow(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupTray();
}

void HistoryWindow::setupUI() {
    // Window flags: Frameless, Tool (not in taskbar), Always on top
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Main Layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10); // Margin for drop shadow effect if we added one, here it serves as padding
    layout->setSpacing(0);

    // Main Container (Card)
    QWidget *container = new QWidget(this);
    container->setObjectName("Container");
    
    // Modern Dark Theme Stylesheet
    QString style = R"(
        #Container {
            background-color: #1E1E2E; 
            border: 1px solid #313244;
            border-radius: 12px;
        }
        QLabel {
            color: #CDD6F4;
            font-family: 'Segoe UI', 'Roboto', sans-serif;
            font-size: 14px;
            font-weight: 600;
            padding: 10px 5px 5px 5px;
        }
        QLineEdit {
            background-color: #313244;
            color: #CDD6F4;
            border: 1px solid #45475A;
            border-radius: 6px;
            padding: 8px;
            font-size: 13px;
            selection-background-color: #585B70;
        }
        QLineEdit:focus {
            border: 1px solid #89B4FA;
            background-color: #313244;
        }
        QListWidget {
            background-color: transparent;
            border: none;
            outline: none;
        }
        QListWidget::item {
            color: #A6ADC8;
            padding: 12px 10px;
            border-bottom: 1px solid #313244;
            margin: 0px 4px;
            border-radius: 4px;
        }
        QListWidget::item:selected {
            background-color: #45475A;
            color: #FFFFFF;
        }
        QListWidget::item:hover {
            background-color: #313244;
            color: #FFFFFF;
        }
        /* Custom Scrollbar */
        QScrollBar:vertical {
            border: none;
            background: #1E1E2E;
            width: 8px;
            margin: 0px 0px 0px 0px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: #45475A;
            min-height: 20px;
            border-radius: 4px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )";
    container->setStyleSheet(style);

    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(15, 15, 15, 15);
    containerLayout->setSpacing(10);

    // Header Title
    headerLabel = new QLabel("Clipboard History", container);
    headerLabel->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(headerLabel);

    // Search Box
    searchBox = new QLineEdit(container);
    searchBox->setPlaceholderText("Search...");
    searchBox->setClearButtonEnabled(true);
    connect(searchBox, &QLineEdit::textChanged, this, &HistoryWindow::onSearchTextChanged);
    containerLayout->addWidget(searchBox);

    // List Widget
    listWidget = new QListWidget(container);
    listWidget->setFrameShape(QFrame::NoFrame);
    listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(listWidget, &QListWidget::itemClicked, this, &HistoryWindow::onItemClicked);
    
    containerLayout->addWidget(listWidget);
    layout->addWidget(container);

    // Set a reasonable size
    resize(400, 500);
}

void HistoryWindow::setupTray() {
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    
    trayMenu = new QMenu(this);
    // Style the menu to match slightly (though Qt menus are hard to style perfectly across platforms)
    trayMenu->setStyleSheet("QMenu { background-color: #1E1E2E; color: white; border: 1px solid #45475A; } QMenu::item:selected { background-color: #45475A; }");

    QAction *showAction = new QAction("Show History", this);
    connect(showAction, &QAction::triggered, this, &HistoryWindow::toggleVisibility);
    
    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    
    trayMenu->addAction(showAction);
    trayMenu->addAction(quitAction);
    
    trayIcon->setContextMenu(trayMenu);
    
    connect(trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason){
        if (reason == QSystemTrayIcon::Trigger) {
            toggleVisibility();
        }
    });
    
    trayIcon->show();
}

void HistoryWindow::updateList(const QList<QString>& history) {
    currentHistory = history;
    refreshView();
}

void HistoryWindow::onSearchTextChanged(const QString &text) {
    Q_UNUSED(text);
    refreshView();
}

void HistoryWindow::refreshView() {
    listWidget->clear();
    QString searchText = searchBox->text().trimmed();

    for (const QString& text : currentHistory) {
        // Filter based on search
        if (!searchText.isEmpty() && !text.contains(searchText, Qt::CaseInsensitive)) {
            continue;
        }

        // Clean text for display
        QString displayText = text;
        
        // Replace newlines with spaces
        displayText = displayText.replace('\n', ' ').replace('\r', ' ');
        
        // Truncate if too long
        if (displayText.length() > 60) {
            displayText = displayText.left(60) + "...";
        }
        
        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, text); // Store full text
        
        // Optional: Set tool tip to show full text on hover
        item->setToolTip(text.left(500)); 
        
        listWidget->addItem(item);
    }
}

void HistoryWindow::onItemClicked(QListWidgetItem *item) {
    QString fullText = item->data(Qt::UserRole).toString();
    emit itemSelected(fullText);
    hide();
    searchBox->clear(); // Clear search on selection
}

void HistoryWindow::toggleVisibility() {
    if (isVisible()) {
        hide();
    } else {
        // Center on cursor or near tray, let's stick to cursor logic
        QPoint cursor = QCursor::pos();
        QRect screenGeo = QGuiApplication::screenAt(cursor)->geometry();
        
        int x = cursor.x() - width() / 2;
        int y = cursor.y() + 10; // Slightly below cursor
        
        // Boundary checks
        if (x < screenGeo.left()) x = screenGeo.left() + 10;
        if (x + width() > screenGeo.right()) x = screenGeo.right() - width() - 10;
        if (y + height() > screenGeo.bottom()) y = cursor.y() - height() - 10; // Show above if too low
        
        move(x, y);
        show();
        activateWindow();
        searchBox->setFocus(); // Focus search box immediately
    }
}

void HistoryWindow::focusOutEvent(QFocusEvent *event) {
    Q_UNUSED(event);
    hide();
}
