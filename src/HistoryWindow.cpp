#include "HistoryWindow.h"
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <QStyle>
#include <QCursor>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionViewItem>
#include <QRect>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QTextOption>
#include <QPushButton>

// Custom Delegate for Windows-style "Cards"
class ClipboardItemDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        // Define the card area (smaller than the list item area to create gaps)
        QRect cardRect = option.rect.adjusted(6, 4, -6, -4);
        
        // Colors (Windows 11 Dark Mode inspired)
        QColor bgColor = QColor("#2D2D2D"); // Card background
        QColor borderColor = QColor("#404040"); // Card border
        QColor textColor = QColor("#FFFFFF"); // Text color
        
        bool isSelected = option.state & QStyle::State_Selected;
        bool isHovered = option.state & QStyle::State_MouseOver;

        if (isSelected) {
            bgColor = QColor("#383838");
            borderColor = QColor("#606060"); // Highlight border
        } else if (isHovered) {
            bgColor = QColor("#323232");
        }

        // Draw Card Background
        QPainterPath path;
        path.addRoundedRect(cardRect, 6, 6);
        
        painter->setPen(QPen(borderColor, 1));
        painter->setBrush(bgColor);
        painter->drawPath(path);

        // Draw Text
        QString fullText = index.data(Qt::UserRole).toString();
        QRect textRect = cardRect.adjusted(12, 12, -40, -12); // Padding + Space for icons on right
        
        painter->setPen(textColor);
        painter->setFont(option.font);
        
        QTextOption textOption;
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        textOption.setAlignment(Qt::AlignLeft | Qt::AlignTop);
        
        // Elide text manually if needed or rely on clipping.
        // QPainter doesn't auto-elide multi-line nicely with drawText in a simple way without layout,
        // but let's just draw it clipped to the rect.
        painter->drawText(textRect, fullText, textOption);

        // Draw "..." Icon (Menu) - Top Right
        painter->setBrush(QColor("#A0A0A0"));
        painter->setPen(Qt::NoPen);
        int iconX = cardRect.right() - 25;
        int iconY = cardRect.top() + 15;
        
        // 3 dots
        painter->drawEllipse(QPoint(iconX, iconY), 2, 2);
        painter->drawEllipse(QPoint(iconX + 6, iconY), 2, 2);
        painter->drawEllipse(QPoint(iconX + 12, iconY), 2, 2);

        // Draw "Pin" Icon placeholder - Bottom Right (simplified as a small rotated rect or circle)
        // Just a visual cue
        // painter->setBrush(Qt::NoBrush);
        // painter->setPen(QPen(QColor("#A0A0A0"), 1.5));
        // painter->drawEllipse(QPoint(iconX + 6, cardRect.bottom() - 15), 5, 5);

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        Q_UNUSED(index);
        Q_UNUSED(option);
        return QSize(option.rect.width(), 80); // Fixed card height
    }
};

HistoryWindow::HistoryWindow(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupTray();
}

void HistoryWindow::setupUI() {
    // Window flags
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Main Layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10); 
    layout->setSpacing(0);

    // Main Container
    QWidget *container = new QWidget(this);
    container->setObjectName("Container");
    
    // Stylesheet (Dark Mode Windows 11-ish)
    QString style = R"(
        #Container {
            background-color: #202020; 
            border: 1px solid #353535;
            border-radius: 8px;
        }
        QLabel {
            color: #FFFFFF;
            font-family: 'Segoe UI', sans-serif;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton {
            background-color: transparent;
            color: #FFFFFF;
            border: 1px solid transparent;
            border-radius: 4px;
            padding: 4px 8px;
            font-size: 12px;
        }
        QPushButton:hover {
            background-color: #353535;
            border: 1px solid #454545;
        }
        QPushButton:pressed {
            background-color: #2D2D2D;
        }
        QLineEdit {
            background-color: #2D2D2D;
            color: #FFFFFF;
            border: 1px solid #3F3F3F;
            border-radius: 4px;
            padding: 6px;
            font-size: 12px;
        }
        QLineEdit:focus {
            border: 1px solid #0078D4; /* Windows Blue */
            background-color: #202020;
        }
        QListWidget {
            background-color: transparent;
            border: none;
            outline: none;
        }
        QScrollBar:vertical {
            border: none;
            background: #202020;
            width: 4px;
            margin: 0px; 
        }
        QScrollBar::handle:vertical {
            background: #505050;
            min-height: 20px;
            border-radius: 2px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )";
    container->setStyleSheet(style);

    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(10, 15, 10, 10);
    containerLayout->setSpacing(10);

    // Header Row (Clipboard Label + Clear All)
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLabel = new QLabel("Clipboard", container);
    headerLayout->addWidget(headerLabel);
    
    headerLayout->addStretch();
    
    QPushButton *clearButton = new QPushButton("Clear all", container);
    clearButton->setCursor(Qt::PointingHandCursor);
    connect(clearButton, &QPushButton::clicked, [this](){
        currentHistory.clear();
        refreshView();
        // Also need to clear backend history via signal or method if fully implemented
    });
    headerLayout->addWidget(clearButton);

    QPushButton *closeButton = new QPushButton("✕", container);
    closeButton->setCursor(Qt::PointingHandCursor);
    closeButton->setFixedSize(24, 24);
    closeButton->setToolTip("Close");
    connect(closeButton, &QPushButton::clicked, this, &HistoryWindow::hide);
    headerLayout->addWidget(closeButton);
    
    containerLayout->addLayout(headerLayout);

    // Optional Search (Kept small/subtle or could remove if user insisted "top section badh" means everything)
    // User said "tob er section ta lagbe na" referring to the emojis tabs in the image.
    // I'll keep search as it's useful but maybe make it part of the content flow or remove to be purely like the image.
    // The image doesn't show a search bar right there. I will Comment it out to strictly match "box box akare text etc".
    /*
    searchBox = new QLineEdit(container);
    searchBox->setPlaceholderText("Search...");
    searchBox->setClearButtonEnabled(true);
    connect(searchBox, &QLineEdit::textChanged, this, &HistoryWindow::onSearchTextChanged);
    containerLayout->addWidget(searchBox);
    */
    // We need to initialize searchBox pointer though to avoid crash in methods using it
    searchBox = new QLineEdit(container); // Hidden/Unused for now
    searchBox->setVisible(false);

    // List Widget
    listWidget = new QListWidget(container);
    listWidget->setFrameShape(QFrame::NoFrame);
    
    // Vertical List Mode (Back from Grid)
    listWidget->setViewMode(QListWidget::ListMode);
    listWidget->setSpacing(0); // Spacing handled by delegate margins
    
    listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Set Custom Delegate
    listWidget->setItemDelegate(new ClipboardItemDelegate(listWidget));
    
    connect(listWidget, &QListWidget::itemClicked, this, &HistoryWindow::onItemClicked);
    
    containerLayout->addWidget(listWidget);
    layout->addWidget(container);

    // Size adjusted (Narrower, taller like the popup)
    resize(360, 500);
}

void HistoryWindow::setupTray() {
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    
    trayMenu = new QMenu(this);
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
        
        // We pass the raw text to the delegate, it handles the "2 lines" display
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::UserRole, text);
        
        // Tooltip still gets the full text
        item->setToolTip(text.left(1024));
        
        listWidget->addItem(item);
    }
}

void HistoryWindow::onItemClicked(QListWidgetItem *item) {
    QString fullText = item->data(Qt::UserRole).toString();
    emit itemSelected(fullText);
    hide();
    searchBox->clear(); 
}

void HistoryWindow::toggleVisibility() {
    if (isVisible()) {
        hide();
    } else {
        QPoint cursor = QCursor::pos();
        QRect screenGeo = QGuiApplication::screenAt(cursor)->geometry();
        
        int x = cursor.x() - width() / 2;
        int y = cursor.y() + 10;
        
        if (x < screenGeo.left()) x = screenGeo.left() + 10;
        if (x + width() > screenGeo.right()) x = screenGeo.right() - width() - 10;
        if (y + height() > screenGeo.bottom()) y = cursor.y() - height() - 10;
        
        move(x, y);
        show();
        activateWindow();
    }
}

void HistoryWindow::focusOutEvent(QFocusEvent *event) {
    Q_UNUSED(event);
    hide();
}

void HistoryWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        hide();
    } else {
        QWidget::keyPressEvent(event);
    }
}
