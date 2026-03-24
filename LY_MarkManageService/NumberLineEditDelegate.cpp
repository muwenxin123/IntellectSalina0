#include "NumberLineEditDelegate.h"

NumberLineEditDelegate::NumberLineEditDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

QWidget *NumberLineEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    QDoubleValidator* editor_DoubleValidator = new QDoubleValidator(editor);
    if(editor && editor_DoubleValidator){
        editor->setFrame(false);
        editor_DoubleValidator->setDecimals(2);
        editor_DoubleValidator->setNotation(QDoubleValidator::StandardNotation);
        editor->setValidator(editor_DoubleValidator);
    }

    return editor;
}

void NumberLineEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QString& value = index.model()->data(index, Qt::EditRole).toString();

    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    if(lineEdit){
        lineEdit->setText(value);
    }
}

void NumberLineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit  = static_cast<QLineEdit*>(editor);
    const QString& value = lineEdit->text();

    model->setData(index, value, Qt::EditRole);
}

void NumberLineEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
