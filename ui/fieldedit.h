#ifndef FIELDEDIT_H
#define FIELDEDIT_H

#include <QLineEdit>

class FieldEdit : public QLineEdit
{
public:
    explicit FieldEdit(QWidget*);
private:
    enum key_handling_kind { regular_key_press, insert_key_press };

    void keyPressEvent(QKeyEvent*);

    key_handling_kind handling;
};

#endif // FIELDEDIT_H
