#ifndef AGRIWORKMACHINE_h__
#define AGRIWORKMACHINE_h__

#include "IAgriComponent.h"

class WorkMachine : public IAgriComponent {
public:
	WorkMachine(int id, const QString& name, const QString& type, double width, double depth);

	int getId() const override;
	QString getName() const override;
	QString getType() const override;

	QString workType;
	double width;
	double depth;

private:
	int id;
	QString name;
};

#endif // AGRIWORKMACHINE_h__
