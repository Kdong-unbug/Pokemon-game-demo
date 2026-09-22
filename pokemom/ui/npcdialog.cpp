#include "npcdialog.h"
#include <QFont>

NpcDialog::NpcDialog(QWidget *parent)
    : QLabel(parent)
{

    setGeometry(180, 720, 900, 200);
    setWordWrap(true);
    setStyleSheet(
        "background-color: rgba(0, 0, 0, 200);"
        "border: 2px solid #FFD700;"
        "border-radius: 10px;"
        "color: white;"
        "font-size: 24px;"
        "padding: 20px;"
    );
    hide();
}

void NpcDialog::setLines(const QStringList &lines)
{
    m_lines = lines;
}

void NpcDialog::setName(const QString &name)
{
    m_name = name;
}

void NpcDialog::open()
{
    if (m_lines.isEmpty()) return;
    m_currentLine = 0;
    m_isOpen = true;
    updateDisplay();
    show();
    raise();
}

void NpcDialog::close()
{
    if (!m_isOpen) return;   // 防止关闭已关闭的对话框时重复触发回调
    m_isOpen = false;
    hide();
    if (m_onFinished) m_onFinished();
    emit finished();
}

bool NpcDialog::isOpen() const
{
    return m_isOpen;
}

void NpcDialog::handleKeyPress(QKeyEvent *event)
{
    if (!m_isOpen) return;

    switch (event->key())
    {
        case Qt::Key_E:
            nextLine();
            break;
        case Qt::Key_Escape:
            this->close();
            break;
        default:
            break;
    }
}

void NpcDialog::setOnFinished(std::function<void()> callback)
{
    m_onFinished = std::move(callback);
}

void NpcDialog::nextLine()
{
    m_currentLine++;
    if (m_currentLine >= m_lines.size())
    {
        this->close();
    } else {
        updateDisplay();
        emit lineChanged(m_currentLine);
    }
}

void NpcDialog::updateDisplay()
{
    QString text = QString("【%1】%2\n\n（按E继续）")
                       .arg(m_name)
                       .arg(m_lines.at(m_currentLine));

    setText(text);
}
