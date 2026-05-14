#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QSpinBox>
#include <QScrollArea>
#include <QMessageBox>
#include <QInputDialog>
#include "memory.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAllocateClicked();
    void onDeallocateClicked();
    void onResetClicked();

private:
    // memory logic
    memory *mem = nullptr;
    vector<process> processes;
    int totalMemSize = 0;

    // ── LEFT PANEL ──
    QWidget     *inputPanel;
    QSpinBox    *memSizeInput;
    QSpinBox    *holeCountInput;
    QTextEdit   *holesInput;
    QComboBox   *strategyCombo;
    QPushButton *addProcessBtn;
    QPushButton *deallocateBtn;
    QPushButton *resetBtn;

    // ── RIGHT PANEL ──
    QWidget     *displayPanel;
    QWidget     *memoryCanvas;
    QTextEdit   *segmentTableDisplay;

    void setupUI();
    void setupLeftPanel();
    void setupRightPanel();
    void initializeMemory();
    void drawMemoryLayout();
    void updateSegmentTable(process& p);
};

#endif