#ifndef ___TRACKLIST_H___
#define ___TRACKLIST_H___

#include <QWidget>
#include <QObject>

class QTableWidget;
class QTrack;

class TrackList : public QWidget
{
    Q_OBJECT

public:
    TrackList();

    void Update(int nID, const QTrack *pTrack);
    void Remove(int nID);
    void RemoveByType(int nID);

private:
    QTableWidget *GetTableWidget(const QString &strZ, const QString &strP, const QTrack *pTrack);

private slots:
    void OnCellDoubleClicked(int row, int column);

private:
    QTableWidget    *m_pListW;
    QTableWidget    *m_pListD;
    QTableWidget    *m_pListX;
};

#endif
