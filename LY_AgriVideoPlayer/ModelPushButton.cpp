#include "ModelPushButton.h"



ModelPushButton::ModelPushButton(const QString ModelTitle, const QString ModelName,
                                 const QString model_cfg, const QString &model_weights,
                                 const QString &model_names, QWidget *parent)
    :QPushButton(ModelTitle, parent)
    ,m_ModelTitle(ModelTitle)
    ,m_ModelName(ModelName)
    ,m_model_cfg(model_cfg)
    ,m_model_weights(model_weights)
    ,m_model_names(model_names)
{
    setText(ModelTitle);

    setStyleSheet(QString("ModelPushButton:checked {border-image: none; background-color: rgb(0, 0, 255); border: none;}"));
    //connect(this, &QPushButton::clicked, this, &ModelPushButton::ModelPushButtonClicked);
}

ModelPushButton::~ModelPushButton()
{

}

const QString &ModelPushButton::getStrModelTitle() const
{
    return  m_ModelTitle;
}

const QString &ModelPushButton::getStrModelName() const
{
    return m_ModelName;
}

const QString &ModelPushButton::getStrmodel_cfg() const
{
    return m_model_cfg;
}

const QString &ModelPushButton::getStrmodel_weights() const
{
    return m_model_weights;
}

const QString &ModelPushButton::getStrmodel_names() const
{
    return m_model_names;
}

void ModelPushButton::ModelPushButtonClicked()
{
    bool check = this->isChecked();
    this->setChecked(check);
}
