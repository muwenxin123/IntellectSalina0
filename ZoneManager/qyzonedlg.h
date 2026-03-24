#ifndef QYZONEDLG_H
#define QYZONEDLG_H

#include <QTreeWidgetItem>
#include <QMenu>
#include <QButtonGroup>

#include "LyDialog.h"
#include "qyzonedbmgr.h"
#include "genstructdef.h"
#include "ui_qyzonedialog.h"

class QPainter;

class CQYZoneDlg: public LyDialog
{
    Q_OBJECT
public:
    CQYZoneDlg(QWidget *parent = nullptr);
    ~CQYZoneDlg();

    enum ZoneType { CIRCLE, PIE, ELLIPSE, POLY, RECTANGLE, CIRCLERING, PIERING, POINT, LINE };

    CXQYZoneTreeWidget *getZoneTreeCtrl()
    {
        return m_QYZoneTree;
    }
    QTableWidget *getZoneParasList()
    {
        return m_listCtrlQYZoneParas;
    }
    void setDwFlashFlag(char curFlag)
    {
        m_DwFlashFlag = curFlag;
    }
    bool isModified()
    {
        return m_bModifyFlag;
    }
    bool isCreated()
    {
        return m_bCreateFlag;
    }
    void setCurSelDotIdx(int curSelIdx)
    {
        m_iCurSelDotIdx = curSelIdx;
    }
    void Refresh();
    void WriteKyList(QUYU_Zone &zone, bool InitFlg = true);
    void insertJWDpt(double ll, double bb, bool bEndflg = false);
    void InsertMouseMoveJWD(double ll, double bb);

    void initUI();
    void setupConnection();
    void DrawQYZoneData(QPainter *painter);
    void DrawOneQuYu(QPainter *pDC, QUYU_Zone &zone, bool bModify = false);
    void DingWeiqy(const QUYU_Zone &zone);

	bool On2d3dChanged(int wParam);

protected:
    virtual void closeEvent(QCloseEvent *e) override;

private:
    void SetModifyState(bool flag, bool Typeflag = true);
    void ChangeListColumn(int m_zoneType);
    void removeTableRow();
    void setRadioState(int);
    bool isNewNameValidate(const QString &name, char zname[]);
    QUYU_Zone GetCurrentValue();
    bool isZoneParaValidate(const QUYU_Zone &zone);
    void InsertDotList(int index, const QString &str, const QString &strJD, const QString &strWD);

    void DrawFG(QPainter *pDC, int x1, int y1, int x2, int y2);
    void DrawLyJLPrompt(QPainter *pDC, const QUYU_Zone &zone);
    void DrawKongYuCircle(QPainter *pDC, double x, double y, float radius, float startangle, float endangle, int type, int filltype);
    void DrawKongYuPie(QPainter *pDC, double x, double y, float radius, float startangle, float endangle, int type, int filltype, QPainterPath &);
    void calcPoint(double x, double y, double l, double h, double angle, double type, double &centerx, double &centery, QVector<QPointF> &pts);
    void DrawKongYuRect(QPainter *pDC, QUYU_Zone *pZone, double x1, double y1, bool flag = 0);
    void DrawQYEllipse(QPainter *pDC, double x, double y, float fLRadius, float fSRadius, float fRotateAngle, int filltype);
    void DrawCircleRing(QPainter *pDC, double x, double y, QUYU_Zone &zone);
    void DrawArcRing(QPainter *pDC, float x, float y, QUYU_Zone &zone);
    void DrawZhuYuan(QPainter *p, const QUYU_Zone &zone, double centerX, double centerY);

private slots:
    void OnBnClickedBtnReset();
    void OnBtnNew();
    void OnBtnNosave();
    void OnBtnModify();
    void OnBtsavezone();
    void OnBnClickedClearItems();
    void OnBnClickedBtnOutputParasToWord();
    void OnRadioClickedSlot();
    void OnTableItemClick(QTableWidgetItem *item);

public:
    CXQYZoneTreeWidget *m_QYZoneTree;
    QTableWidget *m_listCtrlQYZoneParas;
    int m_iCurSelDotIdx;
    bool m_bModifyFlag;
    bool m_bCreateFlag;

private:
    bool m_bMoveInsert;
    bool m_bQYZoneSmooth;
    char m_DwFlashFlag;
    int m_iQYZoneType;
    int m_iQYZoneTopHeight;
    int m_iQYZoneBotHeight;

    Ui::QYZoneDialog *ui;
    QButtonGroup *m_buttonGroup;
    QString m_strQYZoneName;
    QUYU_Zone m_QYZoneBackup;
};

#endif
