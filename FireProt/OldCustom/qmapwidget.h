#ifndef QMapWidget_H
#define QMapWidget_H

#include <QWidget>

class QTabWidget;
class MapIconWidget;
class QLabel;

class QMapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMapWidget(QWidget *parent = nullptr);
	virtual ~QMapWidget();

	void setTips(const QStringList &tips);
	void setWidget(QWidget *p3D, QWidget *p2D);

signals:
	void resizeSig();

protected:
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);

public:

	Q_SLOT void switch23D(bool D3);

public:
	QWidget			*m_pMap;
	QTabWidget		*m_pLayerControl;
	MapIconWidget	*m_pMapIconWidget;
	QLabel			*m_pTipLabel;

private:
	bool			m_b3D;
	QWidget			*m_pEarth;
	QWidget			*m_p2D;
};

#endif
