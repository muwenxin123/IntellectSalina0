#ifndef MODELPUSHBUTTON_H
#define MODELPUSHBUTTON_H

#include <QObject>
#include <QPushButton>

class ModelPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ModelPushButton(const QString ModelTitle, const QString ModelName, const QString model_cfg,
                           const QString &model_weights, const QString &model_names,QWidget *parent = nullptr);
     ~ModelPushButton();

    const QString &getStrModelTitle() const;

    const QString &getStrModelName() const;

    const QString &getStrmodel_cfg() const;

    const QString &getStrmodel_weights() const;

    const QString &getStrmodel_names() const;

private slots:
    void ModelPushButtonClicked();


private:
    const QString m_ModelTitle;
    const QString m_ModelName;
    const QString                m_model_cfg;
    const QString                m_model_weights;
    const QString                m_model_names;
};

#endif // MODELPUSHBUTTON_H
