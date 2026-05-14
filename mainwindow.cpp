#include <QMainWindow>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("Memory Allocation — Segmentation");
    setMinimumSize(900, 600);
    setupUI();

    connect(addProcessBtn,  &QPushButton::clicked, this, &MainWindow::onAllocateClicked);
    connect(deallocateBtn,  &QPushButton::clicked, this, &MainWindow::onDeallocateClicked);
    connect(resetBtn,       &QPushButton::clicked, this, &MainWindow::onResetClicked);
}

MainWindow::~MainWindow() {
    delete mem;
}

void MainWindow::setupUI() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    setupLeftPanel();
    setupRightPanel();
    mainLayout->addWidget(inputPanel, 1);
    mainLayout->addWidget(displayPanel, 2);
}

void MainWindow::setupLeftPanel() {
    inputPanel = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(inputPanel);

    QGroupBox *memGroup = new QGroupBox("Memory Configuration");
    QVBoxLayout *memLayout = new QVBoxLayout(memGroup);
    memLayout->addWidget(new QLabel("Total Memory Size:"));
    memSizeInput = new QSpinBox();
    memSizeInput->setRange(1, 100000);
    memSizeInput->setValue(1000);
    memLayout->addWidget(memSizeInput);
    memLayout->addWidget(new QLabel("Number of Holes:"));
    holeCountInput = new QSpinBox();
    holeCountInput->setRange(0, 20);
    holeCountInput->setValue(3);
    memLayout->addWidget(holeCountInput);
    memLayout->addWidget(new QLabel("Holes (start,size — one per line):"));
    holesInput = new QTextEdit();
    holesInput->setPlaceholderText("0,200\n400,300\n800,200");
    holesInput->setMaximumHeight(100);
    memLayout->addWidget(holesInput);
    layout->addWidget(memGroup);

    QGroupBox *stratGroup = new QGroupBox("Allocation Strategy");
    QVBoxLayout *stratLayout = new QVBoxLayout(stratGroup);
    strategyCombo = new QComboBox();
    strategyCombo->addItem("First Fit");
    strategyCombo->addItem("Best Fit");
    stratLayout->addWidget(strategyCombo);
    layout->addWidget(stratGroup);

    addProcessBtn = new QPushButton("Allocate Process");
    deallocateBtn = new QPushButton("Deallocate Process");
    resetBtn      = new QPushButton("Reset");

    addProcessBtn->setStyleSheet("background-color: #2E75B6; color: white; padding: 8px; font-weight: bold;");
    deallocateBtn->setStyleSheet("background-color: #C55A11; color: white; padding: 8px; font-weight: bold;");
    resetBtn->setStyleSheet("background-color: #555555; color: white; padding: 8px;");

    layout->addWidget(addProcessBtn);
    layout->addWidget(deallocateBtn);
    layout->addWidget(resetBtn);
    layout->addStretch();
}

void MainWindow::setupRightPanel() {
    displayPanel = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(displayPanel);

    QGroupBox *memVisGroup = new QGroupBox("Memory Layout");
    QVBoxLayout *memVisLayout = new QVBoxLayout(memVisGroup);
    QScrollArea *scroll = new QScrollArea();
    memoryCanvas = new QWidget();
    memoryCanvas->setMinimumHeight(150);
    memoryCanvas->setStyleSheet("background-color: #1e1e1e;");
    scroll->setWidget(memoryCanvas);
    scroll->setWidgetResizable(true);
    memVisLayout->addWidget(scroll);
    layout->addWidget(memVisGroup, 1);

    QGroupBox *tableGroup = new QGroupBox("Segment Table");
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    segmentTableDisplay = new QTextEdit();
    segmentTableDisplay->setReadOnly(true);
    segmentTableDisplay->setFont(QFont("Courier New", 10));
    segmentTableDisplay->setStyleSheet("background-color: #1e1e1e; color: #d4d4d4;");
    tableLayout->addWidget(segmentTableDisplay);
    layout->addWidget(tableGroup, 1);
}

void MainWindow::initializeMemory() {
    delete mem;
    processes.clear();
    totalMemSize = memSizeInput->value();

    QString holesText = holesInput->toPlainText().trimmed();

    if (holesText.isEmpty()) {
        mem = new memory(totalMemSize); // whole memory = one big hole
    } else {
        mem = new memory(totalMemSize, true); // manual holes
        QStringList lines = holesText.split('\n', Qt::SkipEmptyParts);
        QStringList invalidLines;
        for (auto& line : lines) {
            QStringList parts = line.split(',');
            if (parts.size() != 2) {
                invalidLines.append(line.trimmed() + " (expected start,size)");
                continue;
            }
            int start = parts[0].trimmed().toInt();
            int size  = parts[1].trimmed().toInt();
            if (start < 0 || size <= 0) {
                invalidLines.append(line.trimmed() + " (negative or zero values)");
                continue;
            }
            if (start + size > totalMemSize) {
                invalidLines.append(line.trimmed() + " (exceeds memory size)");
                continue;
            }
            mem->addHole(start, size);
        }
        if (!invalidLines.isEmpty()) {
            QMessageBox::warning(this, "Invalid Hole Format",
                                 "The following lines were skipped:\n\n" +
                                     invalidLines.join("\n"));
        }
    }
    drawMemoryLayout();
}
void MainWindow::onAllocateClicked() {
    if (mem == nullptr)
        initializeMemory();

    bool ok;

    // get process name
    QString pName = QInputDialog::getText(this, "Process Name",
                                          "Enter process name:", QLineEdit::Normal, "", &ok);
    if (!ok || pName.isEmpty()) return;


    for (auto& p : processes) {
        if (p.name == pName.toStdString()) {
            QMessageBox::warning(this, "Duplicate Process",
                                 "Process " + pName + " already exists!\nPlease use a different name.");
            return;
        }
    }

    // get number of segments
    int n = QInputDialog::getInt(this, "Segments",
                                 "Number of segments:", 1, 1, 20, 1, &ok);
    if (!ok) return;

    process p;
    p.name = pName.toStdString();

    for (int i = 0; i < n; i++) {
        // get segment name
        QString sName = QInputDialog::getText(this, "Segment Name",
                                              QString("Segment %1 name:").arg(i+1), QLineEdit::Normal, "", &ok);
        if (!ok || sName.isEmpty()) return;

        // get segment size
        int sSize = QInputDialog::getInt(this, "Segment Size",
                                         QString("Segment %1 size:").arg(i+1), 1, 1, totalMemSize, 1, &ok);
        if (!ok) return;

        // validation 1 — segment size zero
        if (sSize <= 0) {
            QMessageBox::warning(this, "Invalid Size",
                                 QString("Segment %1 size must be greater than 0.").arg(sName));
            return;
        }


        if (sSize > totalMemSize) {
            QMessageBox::warning(this, "Invalid Size",
                                 QString("Segment %1 size (%2) exceeds total memory size (%3).")
                                     .arg(sName).arg(sSize).arg(totalMemSize));
            return;
        }

        segment s;
        s.name = sName.toStdString();
        s.size = sSize;
        s.start = 0;
        s.type = memType::segment;
        s.processName = p.name;
        p.s.push_back(s);
    }

    // allocate using selected strategy
    int strategy = strategyCombo->currentIndex() + 1;
    if (strategy == 1)
        mem->firstFit(p);
    else
        mem->bestFit(p);

    if (p.allocated) {
        processes.push_back(p);
        updateSegmentTable(p);
        drawMemoryLayout();
    } else {
        QMessageBox::warning(this, "Allocation Failed",
                             QString("Process %1 does not fit in memory!\nNot enough contiguous space for all segments.")
                                 .arg(pName));
    }
}
void MainWindow::onDeallocateClicked() {
    if (mem == nullptr) return;

    bool ok;
    QString pName = QInputDialog::getText(this, "Deallocate",
                                          "Enter process name to deallocate:", QLineEdit::Normal, "", &ok);
    if (!ok || pName.isEmpty()) return;

    for (auto it = processes.begin(); it != processes.end(); ++it) {
        if (it->name == pName.toStdString()) {
            processes.erase(it);
            break;
        }
    }

    mem->deallocation(pName.toStdString());

    // redraw segment tables for remaining processes
    segmentTableDisplay->clear();
    for (auto& p : processes)
        updateSegmentTable(p);

    drawMemoryLayout();
}
void MainWindow::onResetClicked() {
    delete mem;
    mem = nullptr;
    processes.clear();
    segmentTableDisplay->clear();

    QLayout *old = memoryCanvas->layout();
    if (old) {
        QLayoutItem *item;
        while ((item = old->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete old;
    }
}

void MainWindow::updateSegmentTable(process& p) {
    QString table;
    table += QString("--- Segment Table for %1 ---\n").arg(QString::fromStdString(p.name));
    table += QString("%1%2%3\n")
                 .arg("Segment", -15)
                 .arg("Base", -10)
                 .arg("Size", -10);
    table += QString(35, '-') + "\n";
    for (auto& seg : p.s) {
        if (seg.type == memType::segment)
            table += QString("%1%2%3\n")
                         .arg(QString::fromStdString(seg.name), -15)
                         .arg(seg.start, -10)
                         .arg(seg.size, -10);
    }
    table += "\n";
    segmentTableDisplay->append(table);
}
void MainWindow::drawMemoryLayout() {
    QLayout *old = memoryCanvas->layout();
    if (old) {
        QLayoutItem *item;
        while ((item = old->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete old;
    }

    QHBoxLayout *layout = new QHBoxLayout(memoryCanvas);
    layout->setSpacing(2);
    layout->setContentsMargins(5, 5, 5, 5);

    for (auto& block : mem->getBlocks()) {
        QWidget *blockWidget = new QWidget();
        int width = max(50, (block.size * 600) / totalMemSize);
        blockWidget->setFixedWidth(width);
        blockWidget->setFixedHeight(120);

        if (block.type == memType::hole) {
            blockWidget->setStyleSheet(
                "background-color: #3a3a3a; border: 1px solid #666; border-radius: 4px;");
        } else {
            blockWidget->setStyleSheet(
                "background-color: #2E75B6; border: 1px solid #1a5a9a; border-radius: 4px;");
        }

        QVBoxLayout *bLayout = new QVBoxLayout(blockWidget);

        QString label = block.type == memType::hole
                            ? "HOLE"
                            : QString::fromStdString(block.processName + "\n" + block.name);

        QLabel *nameLabel = new QLabel(label);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setStyleSheet("color: white; font-size: 9px; background: transparent; border: none;");
        nameLabel->setWordWrap(true);

        QLabel *sizeLabel = new QLabel(
            QString("%1-%2\nsize:%3")
                .arg(block.start)
                .arg(block.start + block.size - 1)
                .arg(block.size));
        sizeLabel->setAlignment(Qt::AlignCenter);
        sizeLabel->setStyleSheet("color: #aaa; font-size: 8px; background: transparent; border: none;");

        bLayout->addWidget(nameLabel);
        bLayout->addWidget(sizeLabel);
        layout->addWidget(blockWidget);
    }

    layout->addStretch();
}