#include "fieldedit.h"

#include <QMessageBox>
#include <QKeyEvent>

FieldEdit::FieldEdit(QWidget* parent_)
    : QLineEdit(parent_), handling(regular_key_press)
{

}

void FieldEdit::keyPressEvent(QKeyEvent * e)
{
    const int k = e->key();
    const Qt::KeyboardModifiers m = e->modifiers();

    if (handling == regular_key_press)
    {
        if (m & Qt::ControlModifier)
        {
            switch (k)
            {
            case Qt::Key_Minus:
            {
                // Remove current field
                const int pos = cursorPosition();
                QString s = text();
                const int n = s.length();
                int i = pos;
                for (; i < n; ++i)
                    if (s[i] == '$')
                    {
                        break;
                    }
                int j = pos;
                for (; j > 0; --j)
                    if (s[j - 1] == '$')
                    {
                        break;
                    }
                s.remove(j - 1, i - (j - 1));
                setText(s);
            }
                return;
            case Qt::Key_Plus:
                handling = insert_key_press;
                return;
            case Qt::Key_Right:
            {
                // Move to next dollar sign
                const int pos = cursorPosition();
                const QString& s = text();
                const int n = s.length();
                for (int i = pos; i < n; ++i)
                    if (s[i] == '$')
                    {
                        setCursorPosition(i);
                        break;
                    }
            }
                return;
            case Qt::Key_Left:
            {
                // Move to prev dollar sign
                const int pos = cursorPosition();
                const QString& s = text();
                for (int i = pos; i > 0; --i)
                    if (s[i - 1] == '$')
                    {
                        setCursorPosition(i - 1);
                        break;
                    }
            }
                return;
            }
        }
        QLineEdit::keyPressEvent(e);
    }
    else if (handling == insert_key_press)
    {
        if (k == Qt::Key_A)
        {
            QString s = "$a" + text();
            setText(s);
            setCursorPosition(2);
        }
        else
        {
            QLineEdit::keyPressEvent(e);
        }

        handling = regular_key_press;
    }
}
