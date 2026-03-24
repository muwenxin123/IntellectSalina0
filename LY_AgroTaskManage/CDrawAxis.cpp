#include "CDrawAxis.h"
#include "QPainter"
#include "QCString.h"
#include "QDateTime"
#include "LY_AgroTaskManage.h"

#define DRAW_BORDER				8
#define AXIS_HEIGHT				2
#define CURRENT_ARROW			3
#define AUTO_FILL_DAY			10
#define GANTT_ONE_LINE_HEIGHT	25

CDrawAxis::CDrawAxis(QWidget *parent) : QWidget(parent)
{
	m_nAxisWidth = 0;
	m_nStartPointX = 0;
	m_nStartPointY = 0;
	m_nCenterY = 0;

	m_nTimeShowType = 0;
	m_bShowDetail = false;
	m_bShowMouseTips = false;

	InitDebugData();

	setMouseTracking(true);
}

void CDrawAxis::paintEvent(QPaintEvent *event)
{
	QPainter p(this);

	if (m_AgroInfoVec.size() == 0)
	{
		//文字
		QRect textRect(m_nStartPointX, m_nCenterY, 200, 20);
		p.setFont(QFont("Arial", 12));
		p.drawText(textRect, Qt::AlignCenter, tr2("当前盐田无任务。"));

	}
	else
	{
		DrawBackGround(p);
		DrawAxisX(p);
		DrawLegend(p);
		DrawGantt(p);
		DrawCurrentPos(p, 42, false);
	}

	p.restore();
}

void CDrawAxis::wheelEvent(QWheelEvent *event)
{
	//滚动位置
	QPoint qWheelPoint = event->pos();
	
	m_nScaleCenterX = qWheelPoint.rx();

	int nScale = event->angleDelta().ry() / 120;
	if (nScale < 0 )
	{
		m_nZoomStates = -1;
	}
	else if (nScale > 0 )
	{
		m_nZoomStates = 1;
	}

	//倍率
	m_nScale += nScale;

	if (m_nScale < 0)
	{
		m_nScale = 0;
		CalaDrawData();
	}
	else if (m_nScale > 2)
	{
		m_nScale = 2;
	}
	else
	{
		CalaDrawData();
	}
	update();
}

void CDrawAxis::resizeEvent(QResizeEvent* e)
{
	SetScale(0);
	CalaDrawData();
}

void CDrawAxis::mousePressEvent(QMouseEvent* e)
{
	if (Qt::LeftButton != e->button())
	{
		return;
	}

	QPoint qPoint = e->pos();

	//判断是否在甘特图区域内
	for (auto itr : m_EquInfoMap)
	{
		for (auto itrChild : itr.second)
		{
			for (auto itrNYCS : itrChild.InfoVec)
			{
				if ((qPoint.rx() >= itrNYCS.drawRect.left()) &&
					(qPoint.rx() <= (itrNYCS.drawRect.left() + itrNYCS.drawRect.width())) &&
					(qPoint.ry() >= itrNYCS.drawRect.top()) &&
					(qPoint.ry() <= itrNYCS.drawRect.top() + itrNYCS.drawRect.height())
					)
				{
					QString strName = itrNYCS.strName;
					QString strNYCS = itrNYCS.strNYCS;

					if (LY_AgroTaskManage::Get().getCDlgAgroParameterShow()->isVisible() == true)
					{
						LY_AgroTaskManage::Get().getCDlgAgroParameterShow()->hide();
					}

					if (LY_AgroTaskManage::Get().getPDlgAgroSampleParaShow()-> isVisible() == true)
					{
						LY_AgroTaskManage::Get().getPDlgAgroSampleParaShow()->hide();
					}

					if (LY_AgroTaskManage::Get().getPDlgAgroManureParaShow()->isVisible() == true)
					{
						LY_AgroTaskManage::Get().getPDlgAgroManureParaShow()->hide();
					}

					if (LY_AgroTaskManage::Get().getPCDlgScanParaShow()->isVisible() == true)
					{
						LY_AgroTaskManage::Get().getPCDlgScanParaShow()->hide();
					}

					if (itrNYCS.nType == 4 || 
						itrNYCS.nType == 5)
					{
						LY_AgroTaskManage::Get().getCDlgAgroParameterShow()->move(qPoint);
						LY_AgroTaskManage::Get().getCDlgAgroParameterShow()->SetStringName(strName);
						LY_AgroTaskManage::Get().getCDlgAgroParameterShow()->SetStringNYCS(strNYCS);
						LY_AgroTaskManage::Get().getCDlgAgroParameterShow()->SetAgriFieldInfo(m_FieldInfo);
						LY_AgroTaskManage::Get().getCDlgAgroParameterShow()->SetDrugRatioVec(itrNYCS.DrugRatioVec);
						long long llStartTIme = m_FarmInfo.llPlanDate + itrNYCS.nStartDay * 24 * 60 * 60;
						long long llEndTIme = m_FarmInfo.llPlanDate + itrNYCS.nEndDay * 24 * 60 * 60;
						LY_AgroTaskManage::Get().getCDlgAgroParameterShow()->SetWorkTime(llStartTIme,llEndTIme);
						LY_AgroTaskManage::Get().getCDlgAgroParameterShow()->show();
						LY_AgroTaskManage::Get().getCDlgAgroParameterShow()->raise();
					}
					else if (itrNYCS.nType == 3)
					{
						LY_AgroTaskManage::Get().getPDlgAgroManureParaShow()->move(qPoint);
						LY_AgroTaskManage::Get().getPDlgAgroManureParaShow()->SetStringName(strName);
						LY_AgroTaskManage::Get().getPDlgAgroManureParaShow()->SetStringNYCS(strNYCS);
						LY_AgroTaskManage::Get().getPDlgAgroManureParaShow()->SetAgriFieldInfo(m_FieldInfo);
						LY_AgroTaskManage::Get().getPDlgAgroManureParaShow()->SetDrugRatioVec(itrNYCS.DrugRatioVec);
						long long llStartTIme = m_FarmInfo.llPlanDate + itrNYCS.nStartDay * 24 * 60 * 60;
						long long llEndTIme = m_FarmInfo.llPlanDate + itrNYCS.nEndDay * 24 * 60 * 60;
						LY_AgroTaskManage::Get().getPDlgAgroManureParaShow()->SetWorkTime(llStartTIme, llEndTIme);
						LY_AgroTaskManage::Get().getPDlgAgroManureParaShow()->show();
						LY_AgroTaskManage::Get().getPDlgAgroManureParaShow()->raise();
					}
					else if (itrNYCS.nType == 7 ||
						itrNYCS.nType == 1)
					{
						LY_AgroTaskManage::Get().getPCDlgScanParaShow()->move(qPoint);
						LY_AgroTaskManage::Get().getPCDlgScanParaShow()->set_name(strName);
						LY_AgroTaskManage::Get().getPCDlgScanParaShow()->set_explanation(strNYCS);
						LY_AgroTaskManage::Get().getPCDlgScanParaShow()->path_plan(QString::fromLocal8Bit(m_FarmInfo.cName));
						LY_AgroTaskManage::Get().getPCDlgScanParaShow()->show();
						LY_AgroTaskManage::Get().getPCDlgScanParaShow()->raise();
					}
					else
					{
						LY_AgroTaskManage::Get().getPDlgAgroSampleParaShow()->move(qPoint);
						LY_AgroTaskManage::Get().getPDlgAgroSampleParaShow()->SetStringName(strName);
						LY_AgroTaskManage::Get().getPDlgAgroSampleParaShow()->SetStringNYCS(strNYCS);
						LY_AgroTaskManage::Get().getPDlgAgroSampleParaShow()->show();
						LY_AgroTaskManage::Get().getPDlgAgroSampleParaShow()->raise();
					}

					return;
				}
			}
		}
	}

	if (m_nScale == 0)
	{
		return;
	}

	m_bLeftDown = true;

	m_nMousePressX = e->pos().rx();

}

void CDrawAxis::mouseMoveEvent(QMouseEvent* e)
{
	if (m_bLeftDown == true)
	{
		int nCurrentX = e->pos().rx();
		m_nStartPointX = m_nStartPointX + (nCurrentX - m_nMousePressX);
		int nLeftMax = rect().left() + DRAW_BORDER;
		if (m_nStartPointX > nLeftMax)
		{
			m_nStartPointX = nLeftMax;
		}
		int nRightMax = nLeftMax + rect().width() - DRAW_BORDER * 2;
		if ((m_nStartPointX + m_nAxisWidth)< nRightMax)
		{
			m_nStartPointX = nRightMax - m_nAxisWidth;
		}

		m_nMousePressX = e->pos().rx();
		update();
	}
	else
	{
		QPoint qPoint = e->pos();
		for (auto itr : m_EquInfoMap)
		{
			for (auto itrChild : itr.second)
			{
				for (auto itrNYCS : itrChild.InfoVec)
				{
					if ((qPoint.rx() >= itrNYCS.drawRect.left()) &&
						(qPoint.rx() <= (itrNYCS.drawRect.left() + itrNYCS.drawRect.width())) &&
						(qPoint.ry() >= itrNYCS.drawRect.top()) &&
						(qPoint.ry() <= itrNYCS.drawRect.top() + itrNYCS.drawRect.height())
						)
					{
						QString strName = itrNYCS.strName;
						QString strNYCS = itrNYCS.strNYCS;

						return;
					}
				}
			}
		}
	}
}

void CDrawAxis::mouseReleaseEvent(QMouseEvent* e)
{
	if (Qt::RightButton == e->button())
	{

	}
	m_bLeftDown = false;
}

void CDrawAxis::DrawBackGround(QPainter& _Panter)
{
	QPen pen1(Qt::white);
	_Panter.setPen(pen1);
	_Panter.drawRect(m_DrawRect);
}

void CDrawAxis::DrawAxisX(QPainter& _Panter)
{
	QPen pen1(Qt::white);
	_Panter.setPen(pen1);

	QRect AxisRect(m_nStartPointX, m_nStartPointY, m_nAxisWidth - DRAW_BORDER, AXIS_HEIGHT * 2);
	_Panter.setBrush(Qt::white);
	_Panter.drawRect(AxisRect);

	//画箭头 
	QVector<QPointF> JTPoints;
	QPointF point1, point2, point3;
	point1.setX(m_nStartPointX + m_nAxisWidth - DRAW_BORDER );
	point1.setY(m_nCenterY - DRAW_BORDER );
	JTPoints.push_back(point1);

	point2.setX(m_nStartPointX + m_nAxisWidth - DRAW_BORDER );
	point2.setY(m_nCenterY + DRAW_BORDER );
	JTPoints.push_back(point2);

	point3.setX(m_nStartPointX + m_nAxisWidth);
	point3.setY(m_nCenterY);
	JTPoints.push_back(point3);

	_Panter.drawPolygon(JTPoints);

	//文字
	QRect textRect(m_nStartPointX+m_nAxisWidth - 130, m_nCenterY - 30, 200, 20);
	_Panter.setFont(QFont("Arial", 12));
	_Panter.drawText(textRect, Qt::AlignCenter, tr2("时间"));

	//画刻度
	//确定起止刻度
	//int nAxisStartX = m_nStartPointX + DRAW_BORDER * 2;
	//int nAxisStartY = m_nStartPointY;
	//int nAxisEndX = m_nAxisWidth - DRAW_BORDER - DRAW_BORDER * 2;
	//int nAxisEndY = m_nStartPointY;

	int nAxisStartX = m_nStartPointX ;
	int nAxisStartY = m_nStartPointY;
	int nAxisEndX = m_nStartPointX + m_nAxisWidth;
	int nAxisEndY = m_nStartPointY;
	int nDotLineHeight = m_nGanTTStartY - nAxisStartY + m_nEquCount * GANTT_ONE_LINE_HEIGHT;

	//double m_OneDayStep = (nAxisEndX - nAxisStartX)*1.0 / m_nSumDays;
	//if (m_nScale == 0)
	//{
	//	m_OneDayStep = (nAxisEndX - nAxisStartX)*1.0 / nSumDay;
	//}
	//else
	//{
	//	m_OneDayStep = (nAxisEndX - nAxisStartX)*1.0 / nSumDay * m_nScale * 2;
	//}

	int nStep(0),nDay(-1),nChildDay(-1),nChild2Day(-1);
	QRect AxisRectTmp, AxisRectTextTmp;
	QString strTime;
	for (auto itrDraw :m_AgrosZSectionVec)
	{
		if (itrDraw.strName != tr("Temp"))
		{
			//画刻度
			AxisRectTmp.setLeft(nAxisStartX + nStep*m_OneDayStep - AXIS_HEIGHT / 2);
			AxisRectTmp.setTop(nAxisStartY - AXIS_HEIGHT * 10);
			AxisRectTmp.setWidth(AXIS_HEIGHT / 2 * 2);
			AxisRectTmp.setHeight(AXIS_HEIGHT * 10);
			_Panter.drawRect(AxisRectTmp);

			//画虚线
			QPen tempPen;
			tempPen.setStyle(Qt::DotLine);
			tempPen.setColor(Qt::white);
			_Panter.setPen(tempPen);
			_Panter.drawLine(nAxisStartX + nStep*m_OneDayStep, nAxisStartY, nAxisStartX + nStep*m_OneDayStep , nAxisStartY + nDotLineHeight);
			tempPen.setStyle(Qt::SolidLine);
			tempPen.setColor(Qt::white);
			_Panter.setPen(tempPen);

			//写文字
			AxisRectTextTmp.setLeft(nAxisStartX + nStep*m_OneDayStep - AXIS_HEIGHT * 7);
			AxisRectTextTmp.setTop(nAxisStartY + AXIS_HEIGHT * 3);
			AxisRectTextTmp.setWidth(30);
			AxisRectTextTmp.setHeight(AXIS_HEIGHT * 10);
			_Panter.setFont(QFont("Arial", 12));
			_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, itrDraw.strName);

			//写时间
			AxisRectTextTmp.setLeft(nAxisStartX + nStep*m_OneDayStep - 30);
			AxisRectTextTmp.setTop(AxisRectTmp.top() - AXIS_HEIGHT * 10);
			AxisRectTextTmp.setWidth(60);
			AxisRectTextTmp.setHeight(AXIS_HEIGHT * 10);
			_Panter.setFont(QFont("Arial", 10));
			
			//if (m_nScale != 0)
			//{
			//	std::map<int, qnzkna::AgroTask::CAgroRefDataSubInfoVec> DayAndAgroInfoMap;
			//	if (itrDraw.stVec.size() == 0)
			//	{
			//		qnzkna::AgroTask::CAgroRefDataSubInfoVec stAgroInfoVec = GetAgroInfoVecByStates(itrDraw.strName);
			//		for (auto itrAgroChild:stAgroInfoVec)
			//		{
			//			int nDayTemp = abs(itrDraw.nTime) + (itrDraw.nTime + itrAgroChild.nTimeParam);
			//			auto itrDay = DayAndAgroInfoMap.find(nDayTemp);
			//			if (itrDay != DayAndAgroInfoMap.end())
			//			{
			//				itrDay->second.push_back(itrAgroChild);
			//			}
			//			else
			//			{
			//				qnzkna::AgroTask::CAgroRefDataSubInfoVec TempVec;
			//				TempVec.push_back(itrAgroChild);
			//				DayAndAgroInfoMap.insert(std::make_pair(nDayTemp, TempVec));
			//			}
			//		}
			//		for (auto itrDayAndAgroInfo : DayAndAgroInfoMap)
			//		{
			//			//画刻度
			//			AxisRectTmp.setLeft(nAxisStartX + (nStep + itrDayAndAgroInfo.first)*m_OneDayStep - AXIS_HEIGHT / 2);
			//			AxisRectTmp.setTop(nAxisStartY - AXIS_HEIGHT * 4);
			//			AxisRectTmp.setWidth(AXIS_HEIGHT / 2 * 2);
			//			AxisRectTmp.setHeight(AXIS_HEIGHT * 4);
			//			_Panter.drawRect(AxisRectTmp);
			//			//写文字
			//			AxisRectTextTmp.setLeft(nAxisStartX + (nStep + itrDayAndAgroInfo.first)*m_OneDayStep - 30);
			//			AxisRectTextTmp.setTop(nAxisStartY + AXIS_HEIGHT * 10);
			//			AxisRectTextTmp.setWidth(60);
			//			AxisRectTextTmp.setHeight(AXIS_HEIGHT * 30);
			//			_Panter.setFont(QFont("Arial", 9));
			//			QString strTmp(tr(""));
			//			if ( itrDayAndAgroInfo.second.size() > 1 )
			//			{
			//				int nNo = 1;
			//				for (auto itrDayAndAgroInfoChlid : itrDayAndAgroInfo.second)
			//				{
			//					strTmp += QString::number(nNo) + ":" + QString::fromLocal8Bit(itrDayAndAgroInfoChlid.cParamName) + "\n";
			//					nNo++;
			//				}
			//			}
			//			else 
			//			{
			//				auto itrDayAndAgroInfoChlid = itrDayAndAgroInfo.second.begin();
			//				if (itrDayAndAgroInfoChlid->cParamName != "")
			//				{
			//					strTmp = QString::fromLocal8Bit(itrDayAndAgroInfoChlid->cParamName);
			//				}
			//			}
			//			_Panter.drawText(AxisRectTextTmp, Qt::AlignHCenter |Qt::AlignTop, strTmp);
			//		}
			//	}
			//}
		}

		if (m_nTimeShowType == 0)
		{
			if ( itrDraw.strName == tr2("种植"))
			{
				//_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, tr2("T0"));
				strTime = tr2("T0");
				nDay = abs(itrDraw.nTime);
				nChildDay = nDay;
			}
			else
			{
				if (nDay != -1)
				{
					//_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, QString::number(nDay));
					strTime = QString::number(nDay);
					nChildDay = nDay;
					nDay = nDay + abs(itrDraw.nTime);
				}
				else
				{
					strTime = QString::number(itrDraw.nTime);
					//_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, QString::number(itrDraw.nTime));
				}
			}
		}
		else if (m_nTimeShowType == 1)
		{
			_Panter.setFont(QFont("Arial", 9));
			if (itrDraw.strName == tr2("种植"))
			{
				strTime =  QDateTime::fromSecsSinceEpoch(m_FarmInfo.llPlanDate).toString("yy/MM/dd");
				//_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, strTime);
				nDay = abs(itrDraw.nTime);
				nChildDay = nDay;
			}
			else
			{
				if (nDay != -1)
				{
					strTime = QDateTime::fromSecsSinceEpoch(m_FarmInfo.llPlanDate + nDay * 24 * 60 * 60).toString("yy/MM/dd");
					//_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, strTime);
					nChildDay = nDay;
					nDay = nDay + abs(itrDraw.nTime);
				}
				else
				{
					strTime = QDateTime::fromSecsSinceEpoch(m_FarmInfo.llPlanDate+ itrDraw.nTime*24*60*60).toString("yy/MM/dd");
					//_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, strTime);
				}
			}
		}
		if (itrDraw.strName != tr("Temp"))
		{
			_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, strTime);
		}

		if (m_bShowDetail == true)
		{
			int nChildStep = nStep;
			int nIndex = 0;
			for (auto itrChlid1 : itrDraw.stVec)
			{
				//画刻度
				AxisRectTmp.setLeft(nAxisStartX + nChildStep*m_OneDayStep - AXIS_HEIGHT / 2);
				AxisRectTmp.setTop(nAxisStartY - AXIS_HEIGHT * 7);
				AxisRectTmp.setWidth(AXIS_HEIGHT / 2 * 2);
				AxisRectTmp.setHeight(AXIS_HEIGHT * 7);
				_Panter.drawRect(AxisRectTmp);

				//画虚线				
				QPen tempPen;
				tempPen.setStyle(Qt::DotLine);
				tempPen.setColor(Qt::white);
				_Panter.setPen(tempPen);
				_Panter.drawLine(nAxisStartX + nChildStep*m_OneDayStep, nAxisStartY, nAxisStartX + nChildStep*m_OneDayStep, nAxisStartY + nDotLineHeight);
				tempPen.setStyle(Qt::SolidLine);
				tempPen.setColor(Qt::white);
				_Panter.setPen(tempPen);

				//写文字
				int nNextX = nAxisStartX + m_OneDayStep*(nChildStep + abs(itrChlid1.nTime)) - AXIS_HEIGHT / 2 ;
				int nCenterTmp = (nNextX + AxisRectTmp.left()) / 2.0;
				AxisRectTextTmp.setLeft(nCenterTmp - 30);
				AxisRectTextTmp.setTop(nAxisStartY + AXIS_HEIGHT * 23);
				AxisRectTextTmp.setWidth(60);
				AxisRectTextTmp.setHeight(AXIS_HEIGHT * 10);
				_Panter.setFont(QFont("Arial", 11));
				_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, itrChlid1.strName);

				//写时间
				AxisRectTextTmp.setLeft(AxisRectTmp.left() - 30);
				AxisRectTextTmp.setTop(AxisRectTmp.top() - AXIS_HEIGHT * 10);
				AxisRectTextTmp.setWidth(60);
				AxisRectTextTmp.setHeight(AXIS_HEIGHT * 10);

				_Panter.setFont(QFont("Arial", 10));
				if (m_nTimeShowType == 0)
				{
					if (nDay != -1)
					{
						if (nIndex != 0)
						{
							_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, QString::number(nChildDay));
						}
						nChild2Day = nChildDay;
						nChildDay += abs(itrChlid1.nTime);
					}
					else
					{
						_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, QString::number(itrChlid1.nTime));
					}

				}
				else if (m_nTimeShowType == 1)
				{
					_Panter.setFont(QFont("Arial", 9));
					if (nDay != -1)
					{
						strTime = QDateTime::fromSecsSinceEpoch(m_FarmInfo.llPlanDate + nChildDay * 24 * 60 * 60).toString("yy/MM/dd");
						if (nIndex != 0)
						{
							_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, strTime);
						}
						nChild2Day = nChildDay;
						nChildDay += abs(itrChlid1.nTime);

					}
					else
					{
						strTime = QDateTime::fromSecsSinceEpoch(m_FarmInfo.llPlanDate + itrChlid1.nTime * 24 * 60 * 60).toString("yy/MM/dd");
						_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, strTime);
					}
				}
				int nChild2Step = nChildStep;
				int nIndexChild = 0;
				for (auto itrChlid2 : itrChlid1.stVec)
				{
					//画刻度
					AxisRectTmp.setLeft(nAxisStartX + nChild2Step*m_OneDayStep - AXIS_HEIGHT / 2);
					AxisRectTmp.setTop(nAxisStartY - AXIS_HEIGHT * 4);
					AxisRectTmp.setWidth(AXIS_HEIGHT / 2 * 2);
					AxisRectTmp.setHeight(AXIS_HEIGHT * 4);
					_Panter.drawRect(AxisRectTmp);

					//画虚线
					QPen tempPen;
					tempPen.setStyle(Qt::DotLine);
					tempPen.setColor(Qt::white);
					_Panter.setPen(tempPen);
					_Panter.drawLine(nAxisStartX + nChild2Step*m_OneDayStep, nAxisStartY, nAxisStartX + nChild2Step*m_OneDayStep, nAxisStartY + nDotLineHeight);
					tempPen.setStyle(Qt::SolidLine);
					tempPen.setColor(Qt::white);
					_Panter.setPen(tempPen);

					//写文字
					nNextX = nAxisStartX + m_OneDayStep*(nChild2Step + abs(itrChlid2.nTime)) - AXIS_HEIGHT / 2;
					nCenterTmp = (nNextX + AxisRectTmp.left()) / 2.0;
					AxisRectTextTmp.setLeft(nCenterTmp - 30);
					AxisRectTextTmp.setTop(nAxisStartY + AXIS_HEIGHT * 12);
					AxisRectTextTmp.setWidth(60);
					AxisRectTextTmp.setHeight(AXIS_HEIGHT * 10);
					_Panter.setFont(QFont("Arial", 9));
					_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, itrChlid2.strName);

					nChild2Step += abs(itrChlid2.nTime);

					//写时间
					AxisRectTextTmp.setLeft(AxisRectTmp.left() - 30);
					AxisRectTextTmp.setTop(AxisRectTmp.top() - AXIS_HEIGHT * 10);
					AxisRectTextTmp.setWidth(60);
					AxisRectTextTmp.setHeight(AXIS_HEIGHT * 10);

					_Panter.setFont(QFont("Arial", 10));
					if (m_nTimeShowType == 0)
					{
						if (nDay != -1)
						{
							if (nIndexChild != 0)
							{
								_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, QString::number(nChild2Day));
							}
							nChild2Day += abs(itrChlid2.nTime);
						}
						else
						{
							_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, QString::number(itrChlid2.nTime));
						}

					}
					else if (m_nTimeShowType == 1)
					{
						_Panter.setFont(QFont("Arial", 9));
						if (nDay != -1)
						{
							strTime = QDateTime::fromSecsSinceEpoch(m_FarmInfo.llPlanDate + nChild2Day * 24 * 60 * 60).toString("yy/MM/dd");
							if (nIndexChild != 0)
							{
								_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, strTime);
							}
							nChild2Day += abs(itrChlid2.nTime);

						}
						else
						{
							strTime = QDateTime::fromSecsSinceEpoch(m_FarmInfo.llPlanDate + itrChlid2.nTime * 24 * 60 * 60).toString("yy/MM/dd");
							_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, strTime);
						}
					}

					nIndexChild++;
				}
				nChildStep += abs(itrChlid1.nTime);
				nIndex++;
			}
		}
		nStep += abs(itrDraw.nTime);
	}

}

void CDrawAxis::DrawLegend(QPainter& _Panter)
{
	//左上角信息
	QRect AxisRectTextTmp;
	AxisRectTextTmp.setLeft(m_DrawRect.left() + DRAW_BORDER);
	AxisRectTextTmp.setTop(m_DrawRect.top() + DRAW_BORDER);
	AxisRectTextTmp.setWidth(200);
	AxisRectTextTmp.setHeight(AXIS_HEIGHT * 80);

	_Panter.setFont(QFont("Arial", 10));
	QString strTips;
	strTips = tr2("当前种子 : ") + QString::fromLocal8Bit(m_FarmInfo.cSeedNo) + tr("\n");
	strTips += tr2("播种时间 : ") + QDateTime::fromSecsSinceEpoch(m_FarmInfo.llPlanDate).toString("yyyy/MM/dd");
	if (m_nTimeShowType == 0)
	{
		strTips += tr2("\n时间单位 : 天");
	}
	strTips += tr2("\n已完成 : ");
	strTips += tr2("\n进行中 : ");
	strTips += tr2("\n未进行 : ");
	if (m_nScale != 0)
	{
		strTips += tr2("\n当前倍率 : ") + QString::number(m_nScale * 2);
	}
	_Panter.drawText(AxisRectTextTmp, Qt::AlignLeft, strTips);

	//画图例
	QRect qRectTmp;
	qRectTmp.setLeft(58);
	qRectTmp.setTop(59 - m_nTimeShowType*16);
	qRectTmp.setWidth(20);
	qRectTmp.setHeight(10);
	_Panter.setPen(Qt::darkGray);
	_Panter.setBrush(Qt::gray);
	_Panter.drawRect(qRectTmp);

	qRectTmp.setLeft(58);
	qRectTmp.setTop(75 - m_nTimeShowType * 16);
	qRectTmp.setWidth(20);
	qRectTmp.setHeight(10);
	_Panter.setPen(Qt::darkYellow);
	_Panter.setBrush(Qt::yellow);
	_Panter.drawRect(qRectTmp);

	qRectTmp.setLeft(58);
	qRectTmp.setTop(91 - m_nTimeShowType * 16);
	qRectTmp.setWidth(20);
	qRectTmp.setHeight(10);
	_Panter.setPen(Qt::darkGreen);
	_Panter.setBrush(Qt::green);
	_Panter.drawRect(qRectTmp);

	//中间
	AxisRectTextTmp.setLeft(m_DrawRect.left() );
	AxisRectTextTmp.setTop(m_DrawRect.top());
	AxisRectTextTmp.setWidth(m_DrawRect.width());
	AxisRectTextTmp.setHeight(m_DrawRect.height()/4);
	_Panter.setFont(QFont("Arial", 20));
	_Panter.setPen(Qt::green);
	strTips = tr2("当前进度正常");
	_Panter.drawText(AxisRectTextTmp, Qt::AlignCenter, strTips);

	//左下角
	//中间
	strTips = tr2("Tips:\n");
	strTips += tr2("即将进入三叶期 , 请做好如下准备：");
	int nRowCount = 2;
	for (auto itr:m_AgroInfoVec)
	{
		if (itr.nPlanState == 3 &&
			itr.nPlanSubState == 0 &&
			itr.nPlanSsubState == 1 )
		{
			for (auto itrChild :itr.vCAgroRefDataSubInfoVec)
			{
				if (itrChild.nDataType != 6)
				{
					strTips += tr2("\n") + QString::fromLocal8Bit(itrChild.cParamName) + tr2("：") + QString::fromLocal8Bit(itrChild.cHandMethod);
					nRowCount++;
				}
			}
			break;
		}
	}

	AxisRectTextTmp.setLeft(m_DrawRect.left() + DRAW_BORDER);
	AxisRectTextTmp.setTop(m_DrawRect.height() - nRowCount*18 - DRAW_BORDER);
	AxisRectTextTmp.setWidth(m_DrawRect.width() - DRAW_BORDER * 2);
	AxisRectTextTmp.setHeight(nRowCount * 18);
	_Panter.setFont(QFont("Arial", 10));
	_Panter.setPen(Qt::white);
	//_Panter.drawRect(AxisRectTextTmp);
	_Panter.drawText(AxisRectTextTmp, Qt::AlignLeft, strTips);
}

void CDrawAxis::DrawCurrentPos(QPainter& _Panter, const int& nDay, const bool Detentioned)
{
	double dPos = (nDay+AUTO_FILL_DAY) * 1.0 / m_nSumDays;
	int nCurrentPosX = m_nStartPointX + m_nAxisWidth * dPos;
	int nCurrentPosY = m_nStartPointY;

	//画虚线
	int nDotLineHeight = m_nGanTTStartY - m_nStartPointY + m_nEquCount * GANTT_ONE_LINE_HEIGHT;
	QPen tempPen;
	tempPen.setStyle(Qt::DotLine);
	tempPen.setColor(Qt::green);
	_Panter.setPen(tempPen);
	_Panter.drawLine(nCurrentPosX, nCurrentPosY, nCurrentPosX, nCurrentPosY + nDotLineHeight);
	tempPen.setStyle(Qt::SolidLine);
	tempPen.setColor(Qt::white);
	_Panter.setPen(tempPen);

	QRect AxisRect(nCurrentPosX - CURRENT_ARROW, nCurrentPosY - 23, CURRENT_ARROW*2, 18);

	//QPen pen1(Qt::green);
	//_Panter.setPen(pen1);
	if (Detentioned == false)
	{
		_Panter.setBrush(Qt::green);
	}
	else
	{
		_Panter.setBrush(Qt::red);
	}
	_Panter.drawRect(AxisRect);

	//画箭头 
	QVector<QPointF> JTPoints;
	QPointF point1, point2, point3;
	point1.setX(nCurrentPosX);
	point1.setY(nCurrentPosY);
	JTPoints.push_back(point1);

	point2.setX(nCurrentPosX - CURRENT_ARROW * 2);
	point2.setY(nCurrentPosY - CURRENT_ARROW * 3);
	JTPoints.push_back(point2);

	point3.setX(nCurrentPosX + CURRENT_ARROW * 2);
	point3.setY(nCurrentPosY - CURRENT_ARROW * 3);
	JTPoints.push_back(point3);

	_Panter.drawPolygon(JTPoints);
}

qnzkna::AgroTask::CAgroRefDataSubInfoVec CDrawAxis::GetAgroInfoVecByStates(const QString& strType1, const QString& strType2 /*= ""*/, const QString& strType3 /*= ""*/)
{
	for (auto itr: m_AgroInfoVec)
	{
		if (strType2 == "" && strType3 == "")
		{
			if (QString::fromLocal8Bit(itr.cPlanState) == strType1)
			{
				return itr.vCAgroRefDataSubInfoVec;
			}
		}
		else
		{
			if (QString::fromLocal8Bit(itr.cPlanState) == strType1 &&
				QString::fromLocal8Bit(itr.cPlanSubState) == strType2 &&
				QString::fromLocal8Bit(itr.cPlanSsubState) == strType3)
			{
				return itr.vCAgroRefDataSubInfoVec;
			}
		}
	}
}

void CDrawAxis::DrawGantt(QPainter& _Panter)
{
	if (m_EquInfoMap.size() == 0)
	{
		return;
	}
	QRect textRect(m_nStartPointX, m_nGanTTStartY, m_nAxisWidth, 200);

	int nCount = 0;
	for (auto& itr: m_EquInfoMap)
	{
		QString strTitle = "";
		switch (itr.first)
		{
		case 1:
			strTitle = tr2("农机");
			break;
		case 2:
			strTitle = tr2("无人机");
			break;
		case 3:
			strTitle = tr2("设施");
			break;
		default:
			return;
		}
		_Panter.setFont(QFont("Arial", 12));
		_Panter.setPen(Qt::white);
		textRect.setLeft(m_nStartPointX);
		textRect.setTop(m_nGanTTStartY + nCount*GANTT_ONE_LINE_HEIGHT);
		textRect.setWidth(m_nAxisWidth);
		textRect.setHeight(20);
		_Panter.drawText(textRect, Qt::AlignLeft | Qt::AlignTop, strTitle);

		nCount++;

		for (auto& itrChild :itr.second)
		{
			textRect.setLeft(m_nStartPointX);
			textRect.setTop(m_nGanTTStartY + nCount * GANTT_ONE_LINE_HEIGHT);
			textRect.setWidth(m_nAxisWidth);
			textRect.setHeight(20);

			strTitle = tr("    ")+ itrChild.strName;
			_Panter.setFont(QFont("Arial", 12));
			_Panter.setPen(Qt::white);
			_Panter.drawText(textRect, Qt::AlignLeft | Qt::AlignTop, strTitle);

			//画虚线
			QPen tempPen;
			tempPen.setStyle(Qt::DotLine);
			tempPen.setColor(Qt::white);
			_Panter.setPen(tempPen);
			_Panter.drawLine(m_nStartPointX + m_OneDayStep*AUTO_FILL_DAY - DRAW_BORDER , m_nGanTTStartY + nCount * GANTT_ONE_LINE_HEIGHT + 10, 
				m_nAxisWidth - m_OneDayStep*AUTO_FILL_DAY/2, m_nGanTTStartY + nCount * GANTT_ONE_LINE_HEIGHT + 10);
			tempPen.setStyle(Qt::SolidLine);
			tempPen.setColor(Qt::white);
			_Panter.setPen(tempPen);

			for ( auto& itrNYCS : itrChild.InfoVec)
			{
				textRect.setLeft(m_nStartPointX + m_OneDayStep*itrNYCS.nStartDay);
				textRect.setTop(m_nGanTTStartY + nCount * GANTT_ONE_LINE_HEIGHT);
				textRect.setWidth((itrNYCS.nEndDay - itrNYCS.nStartDay)* m_OneDayStep);
				textRect.setHeight(20);
				if (m_bInitGanttRect == true)
				{
					itrNYCS.drawRect = textRect;
				}
				_Panter.setFont(QFont("Arial", 9));

				if ( itrNYCS.nStates == 0)
				{
					_Panter.setPen(Qt::darkGray);
					_Panter.setBrush(Qt::gray);
					_Panter.drawRect(textRect);
					_Panter.setPen(Qt::white);
					_Panter.drawText(textRect, Qt::AlignCenter, itrNYCS.strName);
				}
				else if (itrNYCS.nStates == 1)
				{
					_Panter.setPen(Qt::darkYellow);
					_Panter.setBrush(Qt::yellow);
					_Panter.drawRect(textRect);
					_Panter.setPen(Qt::black);
					_Panter.drawText(textRect, Qt::AlignCenter, itrNYCS.strName);
				}
				else if (itrNYCS.nStates == 2)
				{
					_Panter.setPen(Qt::darkGreen);
					_Panter.setBrush(Qt::green);
					_Panter.drawRect(textRect);
					_Panter.setPen(Qt::black);
					_Panter.drawText(textRect, Qt::AlignCenter, itrNYCS.strName);
				}
			}
			nCount++;

		}

	}
	m_bInitGanttRect = false;
}

void CDrawAxis::InitDebugData()
{
	m_EquInfoMap.clear();
	CEquInfo stEquInfo;
	CEquInfoVec stEquInfoVec;

	stEquInfo.nType = 1;
	stEquInfo.nSubType = 1;
	stEquInfo.strName = tr2("平地机");
	stEquInfo.nCount = 1;

	CEquNYCSInfo stEquNYCSInfo;
	stEquNYCSInfo.strName = tr2("整地");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 0;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 15;
	stEquNYCSInfo.strNYCS = tr2("全田高度差异不大于3厘米。");
	stEquNYCSInfo.nStates = 0;
	stEquNYCSInfo.nType = 1;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquInfoVec.push_back(stEquInfo);

	stEquInfo.nType = 1;
	stEquInfo.nSubType = 2;
	stEquInfo.strName = tr2("深松机");
	stEquInfo.nCount = 1;
	stEquInfo.InfoVec.clear();
	stEquNYCSInfo.strName = tr2("深松");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 15;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 27;
	stEquNYCSInfo.strNYCS = tr2("深松土壤（25-30厘米）。\n行距 60-65厘米，株距 25-30厘米");
	stEquNYCSInfo.nStates = 0;
	stEquNYCSInfo.nType = 1;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquInfoVec.push_back(stEquInfo);

	stEquInfo.nType = 1;
	stEquInfo.nSubType = 3;
	stEquInfo.strName = tr2("播种机");
	stEquInfo.nCount = 1;
	stEquInfo.InfoVec.clear();
	stEquNYCSInfo.strName = tr2("播种");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 5;
	stEquNYCSInfo.strNYCS = tr2("深度控制：甜玉米≤3cm，覆土均匀（误差<1cm）;\n密度调控：4500株，行距60cm±5cm；\n密度优化计算器，按地力等级推荐密度（高肥地4000株 / 亩，行距66cm×株距25cm）");
	stEquNYCSInfo.nStates = 0;
	stEquNYCSInfo.nType = 1;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquInfoVec.push_back(stEquInfo);

	stEquInfo.nType = 1;
	stEquInfo.nSubType = 4;
	stEquInfo.strName = tr2("收割机");
	stEquInfo.nCount = 1;
	stEquInfo.InfoVec.clear();
	stEquNYCSInfo.strName = tr2("收割");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10+ 8+8+8+6;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8 + 8  + 6 + 6;
	stEquNYCSInfo.strNYCS = tr2("收割");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 1;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquInfoVec.push_back(stEquInfo);
	m_EquInfoMap.insert(std::make_pair(1, stEquInfoVec));

	stEquInfoVec.clear();
	stEquInfo.nType = 2;
	stEquInfo.nSubType = 1;
	stEquInfo.strName = tr2("喷洒挂载");
	stEquInfo.nCount = 1;
	stEquInfo.InfoVec.clear();
	stEquNYCSInfo.strName = tr2("除草1");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY +30 + 1;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 5;
	stEquNYCSInfo.strNYCS = tr2("喷施乙草胺+莠去津（50g/亩+150ml/亩），抑制稗草萌发。");
	stEquNYCSInfo.nStates = 0;
	stEquNYCSInfo.nType = 4;
	CDrugRatio stDrugRatio;
	stEquNYCSInfo.DrugRatioVec.clear();
	stDrugRatio.nType = 1;
	stDrugRatio.stDrugName = tr2("乙草胺");
	stDrugRatio.nRaito = 50;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stDrugRatio.nType = 2;
	stDrugRatio.stDrugName = tr2("莠去津");
	stDrugRatio.nRaito = 150;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("除草2");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 1;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 5;
	stEquNYCSInfo.strNYCS = tr2("喷施玉农乐（4%烟嘧磺隆）或苯唑草酮，靶向灭除阔叶草（避开心叶）。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 4;
	stEquNYCSInfo.DrugRatioVec.clear();
	stDrugRatio.nType = 1;
	stDrugRatio.stDrugName = tr2("烟嘧磺隆");
	stDrugRatio.nRaito = 40;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("病虫害防治1");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 6;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 +  6 + 8;
	stEquNYCSInfo.strNYCS = tr2("优先选用吡虫啉、BT乳剂等低毒药剂，抽雄前慎用化学农药。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 5;
	stEquNYCSInfo.DrugRatioVec.clear();
	stDrugRatio.nType = 0;
	stDrugRatio.stDrugName = tr2("虫啉");
	stDrugRatio.nRaito = 30;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("病虫害防治2");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY +30 + 7 + 10 + 8 + 8;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY +30 + 7 + 10 + 8 + 8 + 8;
	stEquNYCSInfo.strNYCS = tr2("撒施BT颗粒剂入心叶。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 5;
	stEquNYCSInfo.DrugRatioVec.clear();
	stDrugRatio.nType = 0;
	stDrugRatio.stDrugName = tr2("BT颗粒");
	stDrugRatio.nRaito = 10;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("病虫害防治3");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY +30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8;
	stEquNYCSInfo.strNYCS = tr2("杀虫剂、杀菌剂和叶面肥混合在一起进行喷施，一般喷1-2次，杀菌剂可以选择苯醚甲环唑、戊唑醇，杀虫剂用啶虫脒、高效氯氰菊酯，叶面肥则推荐磷酸二氢钾。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 5;
	stEquNYCSInfo.DrugRatioVec.clear();
	stDrugRatio.nType = 0;
	stDrugRatio.stDrugName = tr2("啶虫脒");
	stDrugRatio.nRaito = 15;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stDrugRatio.nType = 1;
	stDrugRatio.stDrugName = tr2("戊唑醇");
	stDrugRatio.nRaito = 10;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stDrugRatio.nType = 3;
	stDrugRatio.stDrugName = tr2("磷酸二氢钾");
	stDrugRatio.nRaito = 32;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquInfoVec.push_back(stEquInfo);

	stEquInfo.nType = 2;
	stEquInfo.nSubType = 2;
	stEquInfo.strName = tr2("播撒挂载");
	stEquInfo.nCount = 1;
	stEquInfo.InfoVec.clear();
	stEquNYCSInfo.strName = tr2("施肥1");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 +4;
	stEquNYCSInfo.strNYCS = tr2("播种沟侧施磷酸二铵10kg/亩，与种子隔离5cm防烧苗");
	stEquNYCSInfo.nStates = 0;
	stEquNYCSInfo.nType = 3;
	stEquNYCSInfo.DrugRatioVec.clear();
	stDrugRatio.nType = 0;
	stDrugRatio.stDrugName = tr2("磷酸二铵");
	stDrugRatio.nRaito = 10;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("施肥2");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 ;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 5;
	stEquNYCSInfo.strNYCS = tr2("每亩追施尿素 10-15公斤（促进茎叶生长）。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 3;
	stEquNYCSInfo.DrugRatioVec.clear();
	stDrugRatio.nType = 0;
	stDrugRatio.stDrugName = tr2("尿素");
	stDrugRatio.nRaito = 15;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("施肥3");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 6;
	stEquNYCSInfo.strNYCS = tr2("尿素、钾肥。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 3;
	stEquNYCSInfo.DrugRatioVec.clear();
	stDrugRatio.nType = 0;
	stDrugRatio.stDrugName = tr2("尿素");
	stDrugRatio.nRaito = 10;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stDrugRatio.nType = 0;
	stDrugRatio.stDrugName = tr2("钾肥");
	stDrugRatio.nRaito = 20;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("施肥4");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8 + 5;
	stEquNYCSInfo.strNYCS = tr2("追施1～2次叶面肥，可用商品叶面肥，也可每次每公顷用磷酸二氢钾3.0～7.5kg和尿素7.5kg。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 3;
	stEquNYCSInfo.DrugRatioVec.clear();
	stDrugRatio.nType = 0;
	stDrugRatio.stDrugName = tr2("磷酸二氢钾");
	stDrugRatio.nRaito = 5;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stDrugRatio.nType = 0;
	stDrugRatio.stDrugName = tr2("尿素");
	stDrugRatio.nRaito = 7.5;
	stEquNYCSInfo.DrugRatioVec.push_back(stDrugRatio);
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquInfoVec.push_back(stEquInfo);
	stEquNYCSInfo.DrugRatioVec.clear();

	stEquInfo.nType = 2;
	stEquInfo.nSubType = 3;
	stEquInfo.strName = tr2("多光谱");
	stEquInfo.nCount = 1;
	stEquInfo.InfoVec.clear();
	stEquNYCSInfo.strName = tr2("巡查1");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 4;
	stEquNYCSInfo.strNYCS = tr2("长势诊断：航线高度 30-50m ,航向/旁向重叠率 70%/80%,飞行速度 8-12 m/s。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 7;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("巡查2");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 4;
	stEquNYCSInfo.strNYCS = tr2("病虫预警：航线高度 30-50m ,航向/旁向重叠率 70%/80%,飞行速度 8-12 m/s。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 7;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("巡查3");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 5;
	stEquNYCSInfo.strNYCS = tr2("旱涝/倒伏评估：航线高度 30-50m ,航向/旁向重叠率 70%/80%,飞行速度 8-12 m/s。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 7;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("巡查4");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 ;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8  + 5;
	stEquNYCSInfo.strNYCS = tr2("出苗率与测产：航线高度 30-50m ,航向/旁向重叠率 70%/80%,飞行速度 8-12 m/s。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 7;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("巡查5");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8 ;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8 + 4;
	stEquNYCSInfo.strNYCS = tr2("处方图生成：航线高度 30-50m ,航向/旁向重叠率 70%/80%,飞行速度 8-12 m/s。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 7;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquInfoVec.push_back(stEquInfo);

	m_EquInfoMap.insert(std::make_pair(2, stEquInfoVec));

	stEquInfoVec.clear();

	stEquInfo.nType = 3;
	stEquInfo.nSubType = 1;
	stEquInfo.strName = tr2("灌溉");
	stEquInfo.nCount = 1;
	stEquInfo.InfoVec.clear();
	stEquNYCSInfo.strName = tr2("50%-90%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 24;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30;
	stEquNYCSInfo.strNYCS = tr2("壤持水量＜50%触发干旱警报，＞90%触发涝渍风险。");
	stEquNYCSInfo.nStates = 0;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("60%-70%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7;
	stEquNYCSInfo.strNYCS = tr2("土壤持水量60%-70%。");
	stEquNYCSInfo.nStates = 0;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("50%-60%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8;
	stEquNYCSInfo.strNYCS = tr2("甜玉米苗期根系浅弱，玉米播种后至出苗要注意水分管理，出苗后适当控水（持水量50%-60%）促根系下扎。");
	stEquNYCSInfo.nStates = 1;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("70%-80%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10;
	stEquNYCSInfo.strNYCS = tr2("持水量＜65%触发滴灌（40-50立方米/亩），＞85%启动排水系统。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("70%-80%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8;
	stEquNYCSInfo.strNYCS = tr2("持水量＜65%触发滴灌（40-50立方米/亩），＞85%启动排水系统。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("80%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8 + 8 + 6;
	stEquNYCSInfo.strNYCS = tr2("＞85%启动排水系统。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquInfoVec.push_back(stEquInfo);

	stEquInfo.nType = 3;
	stEquInfo.nSubType = 2;
	stEquInfo.strName = tr2("气象");
	stEquInfo.nCount = 1;
	stEquInfo.InfoVec.clear();
	stEquNYCSInfo.strName = tr2("12℃以上");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 15;
	stEquNYCSInfo.strNYCS = tr2("满足温度条件（5cm地温稳定在12℃以上）时进行，大约持续7-15天。");
	stEquNYCSInfo.nStates = 1;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("12℃以上");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 15;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8 + 8 + 6;
	stEquNYCSInfo.strNYCS = tr2("");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquInfoVec.push_back(stEquInfo);

	stEquInfo.nType = 3;
	stEquInfo.nSubType = 3;
	stEquInfo.strName = tr2("苗情(叶龄)");
	stEquInfo.nCount = 1;
	stEquInfo.InfoVec.clear();
	stEquNYCSInfo.strName = tr2("10%-15%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10;
	stEquNYCSInfo.strNYCS = tr2("出苗期:幼苗出土高约2cm，需防涝防板结，促进根系下扎。");
	stEquNYCSInfo.nStates = 1;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("20%-25%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY +30 + 7 + 10;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8;
	stEquNYCSInfo.strNYCS = tr2("三叶期:种子营养耗尽（离乳期），需轻灌水促根，防苗枯病；玉米苗期怕涝不怕旱，轻度的干旱，有利于根系的发育和下扎。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("30%-35%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 ;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 ;
	stEquNYCSInfo.strNYCS = tr2("拔节期:茎节伸长（雄穗分化启动），追施拔节肥（氮肥占总量40%）促茎秆健壮。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("46%-49%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 ;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10;
	stEquNYCSInfo.strNYCS = tr2("小喇叭口期:雌穗伸长、雄穗小花分化，保障水分（持水量70%–80%）防干旱。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("60%-65%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10;
	stEquNYCSInfo.strNYCS = tr2("大喇叭口期:雌雄穗分化关键期，重施穗肥，决定穗粒数的关键期。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("92%-96%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8;
	stEquNYCSInfo.strNYCS = tr2("抽雄期:雄穗抽出顶叶，需防旱保授粉，避免高温导致花粉败育。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("97%-100%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8;
	stEquNYCSInfo.strNYCS = tr2("开花吐丝期:散粉与吐丝同步，保障水分防高温，确保授粉率。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquNYCSInfo.strName = tr2("100%");
	stEquNYCSInfo.nStartDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8;
	stEquNYCSInfo.nEndDay = AUTO_FILL_DAY + 30 + 7 + 10 + 8 + 10 + 10 + 10 + 8 + 8 + 8;
	stEquNYCSInfo.strNYCS = tr2("乳熟期:籽粒灌浆关键期，需防倒伏防早衰，保持绿叶功能。");
	stEquNYCSInfo.nStates = 2;
	stEquNYCSInfo.nType = 2;
	stEquInfo.InfoVec.push_back(stEquNYCSInfo);
	stEquInfoVec.push_back(stEquInfo);

	m_EquInfoMap.insert(std::make_pair(3, stEquInfoVec));
}

void CDrawAxis::CalaDrawData()
{
	m_DrawRect = rect().adjusted(0, 0, -1, -1);
	m_nCenterY = m_DrawRect.height() / 4;
	m_nAxisWidth = m_DrawRect.width() - DRAW_BORDER * 2;
	if (m_nScale != 0)
	{
		if (m_nZoomStates > 0)
		{
			m_nAxisWidth = m_nAxisWidth * m_nScale * 2;
		}
		else if (m_nZoomStates < 0)
		{
			m_nAxisWidth = m_nAxisWidth * m_nScale * 2;
		}
	}

	//放大
	if (m_nScale == 0)
	{
		m_nStartPointX = m_DrawRect.left() + DRAW_BORDER;
	}
	else if (m_nZoomStates > 0)
	{
		m_nStartPointX = m_nStartPointX - (m_nScaleCenterX  - m_nStartPointX);
	}
	else if (m_nZoomStates < 0)
	{
		m_nStartPointX = (m_nStartPointX  + (m_nScaleCenterX )) /2.0;
	}
	//if (m_nScale == 1)
	//{
	//	m_nStartPointX = m_nStartPointX - (m_nScaleCenterX - DRAW_BORDER);
	//}
	//else if (m_nScale == 2)
	//{
	//	m_nStartPointX = m_nStartPointX - (m_nScaleCenterX - DRAW_BORDER)*3;
	//}
	m_nStartPointY = m_nCenterY - AXIS_HEIGHT;
	m_nGanTTStartY = m_DrawRect.height() *38 / 100 - AXIS_HEIGHT;

	m_OneDayStep = m_nAxisWidth * 1.0 / m_nSumDays;

	m_nEquCount = 0;
	for (auto itr : m_EquInfoMap )
	{
		m_nEquCount++;
		for (auto itrChild : itr.second)
		{
			m_nEquCount++;
		}
	}

	m_bInitGanttRect = true;
}

void CDrawAxis::SetFarmlandAttributeInfo(qnzkna::AgroTask::FarmlandAttributeInfo& stInfo)
{
	m_FarmInfo = stInfo;
}

void CDrawAxis::SetAgroRefDataInfoVec(qnzkna::AgroTask::CAgroRefDataInfoVec& InfoVec)
{
	m_AgroInfoVec = InfoVec;
}

void CDrawAxis::SetTimeShowType(const int& nType)
{
	m_nTimeShowType = nType;
}

void CDrawAxis::SetShowDetailFlag(const bool& bFlag)
{
	m_bShowDetail = bFlag;
}

void CDrawAxis::SetShowMouseTipsFlag(const bool& bFlag)
{
	m_bShowMouseTips = bFlag;
}

void CDrawAxis::ArrangeData()
{
	m_AgrosZSectionVec.clear();

	if (m_AgroInfoVec.size() == 0)
	{
		return;
	}

	//填充
	CAgrosZSection stTmp1;
	stTmp1.strName = tr("Temp");
	stTmp1.nTime = AUTO_FILL_DAY;
	m_AgrosZSectionVec.push_back(stTmp1);

	//整理数据
	std::map<QString,int> ;
	for (auto itr :m_AgroInfoVec)
	{
		if (QString::fromLocal8Bit( itr.cPlanSsubState) != "")
		{
			auto itrStates = m_AgrosZSectionVec.begin();
			for (;itrStates != m_AgrosZSectionVec.end();itrStates++)
			{
				if (itrStates->strName == QString::fromLocal8Bit(itr.cPlanState))
				{
					auto itrStates1 = itrStates->stVec.begin();
					for (; itrStates1 != itrStates->stVec.end(); itrStates1++)
					{
						//找到子状态
						if (itrStates1->strName == QString::fromLocal8Bit(itr.cPlanSubState))
						{
							CAgrosZZZSection stAgrosZZZSection;
							stAgrosZZZSection.nTime = abs(itr.nDataTime);
							stAgrosZZZSection.strName = QString::fromLocal8Bit(itr.cPlanSsubState);

							itrStates1->stVec.push_back(stAgrosZZZSection);
							itrStates1->nTime += abs(itr.nDataTime);
							
							break;
						}
					}

					//未找到子状态
					if (itrStates1 == itrStates->stVec.end())
					{
						CAgrosZZZSection stAgrosZZZSection;
						stAgrosZZZSection.nTime = abs(itr.nDataTime);
						stAgrosZZZSection.strName = QString::fromLocal8Bit(itr.cPlanSsubState);

						CAgrosZZSection stAgrosZZSection;
						stAgrosZZSection.nTime = abs(itr.nDataTime);
						stAgrosZZSection.strName = QString::fromLocal8Bit(itr.cPlanSubState);
						stAgrosZZSection.stVec.push_back(stAgrosZZZSection);

						itrStates->stVec.push_back(stAgrosZZSection);
					}
					itrStates->nTime += itr.nDataTime;
					break;
				}
			}
		
			if (itrStates == m_AgrosZSectionVec.end())
			{
				CAgrosZZZSection stAgrosZZZSection;
				stAgrosZZZSection.nTime = itr.nDataTime;
				stAgrosZZZSection.strName = QString::fromLocal8Bit(itr.cPlanSsubState);

				CAgrosZZSection stAgrosZZSection;
				stAgrosZZSection.nTime = itr.nDataTime;
				stAgrosZZSection.strName = QString::fromLocal8Bit(itr.cPlanSubState);
				stAgrosZZSection.stVec.push_back(stAgrosZZZSection);

				CAgrosZSection stAgrosZSection;
				stAgrosZSection.nTime = itr.nDataTime;
				stAgrosZSection.strName = QString::fromLocal8Bit(itr.cPlanState);
				stAgrosZSection.stVec.push_back(stAgrosZZSection);

				m_AgrosZSectionVec.push_back(stAgrosZSection);

			}
		}
		else
		{

			CAgrosZSection stAgrosZSection;
			stAgrosZSection.nTime = itr.nDataTime;
			stAgrosZSection.strName = QString::fromLocal8Bit(itr.cPlanState);

			m_AgrosZSectionVec.push_back(stAgrosZSection);
		}
	}

	m_AgrosZSectionVec.push_back(stTmp1);

	//计算总时长
	m_nSumDays = 0;
	for (auto itr : m_AgrosZSectionVec)
	{
		m_nSumDays += abs(itr.nTime);
	}
}

void CDrawAxis::SetScale(const int& nScale)
{
	if (nScale < -2)
	{
		m_nScale = -2;
	}
	else if (nScale > 2)
	{
		m_nScale = 2;
	}
	else
	{
		m_nScale = nScale;
	}

	CalaDrawData();
}

void CDrawAxis::SetAgriFieldInfo(const AgriFieldInfo& stInfo)
{
	m_FieldInfo = stInfo;
}
