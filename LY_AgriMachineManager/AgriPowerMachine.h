#ifndef AGRIPOWERMACHINE_h__
#define AGRIPOWERMACHINE_h__

#include "IAgriComponent.h"

class PowerMachine : public IAgriComponent {
public:
	PowerMachine(int id, const QString& name, const QString& engineModel, double power);

	int getId() const override;
	QString getName() const override;
	QString getType() const override;

	QString engineModel;
	double power;

private:
	int id;
	QString name;
};

#endif // AGRIPOWERMACHINE_h__
