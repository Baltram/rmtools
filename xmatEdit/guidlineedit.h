#pragma once

#include <QLineEdit>
#include <algorithm>
#include <QApplication>

class GuidLineEdit : public QLineEdit
{
    Q_OBJECT

    public:
        GuidLineEdit(QWidget * parent = 0) : QLineEdit(parent)
        {
            lastCursorPos = cursorPosition();
            connect(this, SIGNAL(textChanged(QString)), this, SLOT(onNonGuidTextChanged(QString)));
        }

        void enableGuidMode(void)
        {
            disconnect(this, SIGNAL(textChanged(QString)), this, SLOT(onNonGuidTextChanged(QString)));
			setInputMask(">XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;f");
            connect(this, SIGNAL(textEdited(QString)), this, SLOT(onTextChanged(QString)));
            connect(this, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(onCursorPosChanged(int,int)));
            onTextChanged("");
        }

        ~GuidLineEdit() {}

    signals:
        void guidChanged(const QString &);

    private slots:
        void onNonGuidTextChanged(QString text)
        {
            if (text == "#GUID") {
                enableGuidMode();
            }
            if (text == "#NOGUID") {
                disconnect(this, SIGNAL(textChanged(QString)), this, SLOT(onNonGuidTextChanged(QString)));
            }
        }

        void onTextChanged(QString text)
        {
            blockSignals(true);
            QRegExp noHex("[^0123456789abcdefABCDEF]");
            int cursorPos = cursorPosition();

            int shift = (cursorPos - lastCursorPos);
            int removeCount = 0;

            if ((text.mid((cursorPos - 1), 1).contains(noHex)) && (shift == 1)) { // User has typed a non-hex-character.
                text[cursorPos - 1] = '0';
                --cursorPos;
                QApplication::beep();
            }
            else if (shift == (-1)) {
                if ((cursorPos == 9) || (cursorPos == 14) || (cursorPos == 19) || (cursorPos == 24)) { // User has pressed backspace and went onto a '-' that way.
                    text[cursorPos - 1] = text[cursorPos];
                    text[cursorPos] = '-';
                    --cursorPos;
                }
            }
            else if (shift > 1) { // User has pasted (but not replaced a selection).
                // Killing non-hex chars.
                QString pasted = text.mid(lastCursorPos, shift);
                if (pasted.contains(noHex)) {
                    pasted.replace(noHex, "0");
                    QApplication::beep();
                }
                text.replace(lastCursorPos, shift, pasted);
                // User may have overwritten some '-' instead of hex characters.
                removeCount = (6 - text.count(noHex)); // Don't delete right away because there might be more '-'.
            }

            cursorPos -= text.left(cursorPos).count(noHex);

            text.remove(noHex);
            text.append("00000000000000000000000000000000");
            text.remove(cursorPos, removeCount);
            text.resize(32);
            text.insert(0, '{').insert(9, '-').insert(14, '-').insert(19, '-').insert(24, '-').append('}');

            cursorPos += ((cursorPos > 19) ? 5 : ((cursorPos > 15) ? 4 : ((cursorPos > 11) ?  3 : ((cursorPos > 7) ?  2 : 1))));

            setText(text.toUpper());
            blockSignals(false);
            setCursorPosition(cursorPos);

            emit guidChanged(text);
        }

        void onCursorPosChanged(int oldP, int newP)
        {
            if (selectedText().size() != 0) {
                setSelection(0, 38);
                return;
            }

            int shift = (newP >= oldP) ? 1 : (-1);
            switch (newP)
            {
                case 0:
                {
                    ++newP;
                    break;
                }
                case 9:
                {
                    newP += shift;
                    break;
                }
                case 14:
                {
                    newP += shift;
                    break;
                }
                case 19:
                {
                    newP += shift;
                    break;
                }
                case 24:
                {
                    newP += shift;
                    break;
                }
                default:
                {
                    newP = std::min(newP, 36);
                }
            }
            lastCursorPos = newP;
            setCursorPosition(newP);
        }

        private:
        int lastCursorPos;
};
