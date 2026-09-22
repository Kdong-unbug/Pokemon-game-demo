#ifndef NPC_DIALOG_H
#define NPC_DIALOG_H

#include <QLabel>
#include <QStringList>
#include <QKeyEvent>
#include <functional>

class NpcDialog : public QLabel
{
    Q_OBJECT

public:
    explicit NpcDialog(QWidget *parent = nullptr);

    /// 设置对话内容（支持多段）
    void setLines(const QStringList &lines);

    /// 设置NPC名称
    void setName(const QString &name);

    /// 显示对话框并开始对话
    void open();

    /// 关闭对话框
    void close();

    /// 是否正在对话中
    bool isOpen() const;

    /// 处理按键（在 mainwindow 的 keyPressEvent 中调用）
    void handleKeyPress(QKeyEvent *event);

    /// 对话结束时的回调
    void setOnFinished(std::function<void()> callback);

signals:
    void finished();          // 所有对话结束时发出
    void lineChanged(int index); // 每切换一句时发出

private slots:
    void nextLine();

private:
    QString m_name;
    QStringList m_lines;
    int m_currentLine = 0;
    bool m_isOpen = false;
    std::function<void()> m_onFinished;

    void updateDisplay();
};

#endif // NPC_DIALOG_H
