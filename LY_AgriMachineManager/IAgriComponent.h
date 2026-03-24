#ifndef IAGRICOMPONENT_h__
#define IAGRICOMPONENT_h__

#include <QString>

class IAgriComponent {
public:
	virtual ~IAgriComponent() {}
	virtual int getId() const = 0;
	virtual QString getName() const = 0;
	virtual QString getType() const = 0;
};
#endif // IAGRICOMPONENT_h__
