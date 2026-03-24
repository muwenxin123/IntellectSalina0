#include "ISSendVideoThread.h"
#include <QDebug>
#include <QDir>
#include "LY_ScenariosReplay.h"
#include "ScenarioReplayDialog.h"

ISSendVideoThread::ISSendVideoThread()
{

}

ISSendVideoThread::~ISSendVideoThread()
{
	stop();
}
void ISSendVideoThread::pause()
{
	if (this)
	{
		is_runing = false;
	}

}

void ISSendVideoThread::stop()
{
	if (this)
	{
		is_runing = false;
		quit();
		wait();
	}

}
void ISSendVideoThread::setVideoPath(const set<LogDataInfo> dataArray, const int port, const int type)
{
	playType = type;
	vector<QString>vec;
	for (auto itr: dataArray)
	{
		QStringList list = itr.fileName.split("_");
		if (list.size() > 1)
		{
			QString a = list.last();
			vec.push_back(a);

			QStringList list1 = a.split(".");
			if (list1.size() > 0)
			{
				QString b = list1.at(0);
				if (b.toInt() == port)
				{
					videoPathMap.insert(make_pair(b.toInt(), itr.fileName));
				}
			}
		}
	}
}

void ISSendVideoThread::run()
{
	is_runing = true;

	int buf_size = 2000000;
	std::unique_ptr<uint8_t> frame_buf(new uint8_t[buf_size]);
	QByteArray ba;
	char szT[256];
	szT[0] = 0x47;
	QUdpSocket udpSocket;
	udpSocket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);

	if (videoPathMap.size() == 0)
	{
		return;
	}

	for (auto itr: videoPathMap)
	{
		if (!is_runing)
		{
			udpSocket.abort();
			udpSocket.close();
			break;
		}
		if (!h264File.Open(itr.second.toLatin1().data()))
		{

			qDebug() << "Open video file failed";
			continue;
		}

		while (true)
		{
			if (!is_runing)
			{
				udpSocket.abort();
				udpSocket.close();
				break;
			}
			bool end_of_frame = false;
			int frame_size = h264File.ReadFrame((char *)frame_buf.get(), buf_size, &end_of_frame);
			if (frame_size > 0)
			{
				char *pT = (char *)frame_buf.get();
				while (frame_size >= 255)
				{
					memcpy(szT + 1, pT, 255);

					if (LY_ScenariosReplay::Get().m_pScenarioReplayDialog->isSeatReplay == false)
					{
						if (playType == 3)
						{
							udpSocket.writeDatagram(szT, 256, QHostAddress("226.0.0.80"), itr.first);
						}
						else
						{
							if (itr.first % 1000 == 0)
							{
								udpSocket.writeDatagram(szT, 256, QHostAddress("226.0.0.80"), itr.first + 1);
							}
							else {
								udpSocket.writeDatagram(szT, 256, QHostAddress("226.0.0.80"), itr.first);
							}
						}
					}
					else
					{
						if (playType == 3)
						{
							udpSocket.writeDatagram(szT, 256, QHostAddress("127.0.0.1"), itr.first);
						}
						else
						{
							if (itr.first % 1000 == 0)
							{

								udpSocket.writeDatagram(szT, 256, QHostAddress("127.0.0.1"), itr.first + 1);
							}
							else {
								udpSocket.writeDatagram(szT, 256, QHostAddress("127.0.0.1"), itr.first);
							}
						}
					}

					pT += 255;
					frame_size -= 255;
				}
				if (frame_size > 0)
				{
					memcpy(szT + 1, pT, frame_size);
					memset(szT + 1 + frame_size, 0, 255 - frame_size);

					if (LY_ScenariosReplay::Get().m_pScenarioReplayDialog->isSeatReplay == false)
					{
						if (playType == 3)
						{
							udpSocket.writeDatagram(szT, 256, QHostAddress("226.0.0.80"), itr.first);
						}
						else
						{
							if (itr.first % 1000 == 0)
							{
								udpSocket.writeDatagram(szT, 256, QHostAddress("226.0.0.80"), itr.first + 1);
							}
							else {
								udpSocket.writeDatagram(szT, 256, QHostAddress("226.0.0.80"), itr.first);
							}
						}
					}
					else
					{
						if (playType == 3)
						{
							udpSocket.writeDatagram(szT, 256, QHostAddress("127.0.0.1"), itr.first);
						}
						else
						{
							if (itr.first % 1000 == 0)
							{
								udpSocket.writeDatagram(szT, 256, QHostAddress("127.0.0.1"), itr.first + 1);
							}
							else {
								udpSocket.writeDatagram(szT, 256, QHostAddress("127.0.0.1"), itr.first);
							}
						}
					}

					int k = 0;
				}
			}
			else
			{
				break;
			}
			QThread::msleep(20);
		}
	}
}
void ISSendVideoThread::closeFile()
{
	h264File.Close();
}
int ISSendVideoThread::read_one_frame(FILE *fp, unsigned char*ptr)
{
	int size = 0;
	static unsigned char ts[4] = { 0 };
	if (fread(ptr, 1, 4, fp) < 4)
	{
		return size;
	}
	if ((*ptr == 0x00) && (*(ptr + 1) == 0x00) && (*(ptr + 2) == 0x00) && (*(ptr + 3) == 0x01))
	{
		size = 4;
		while (1)
		{
			if (fread(ptr + size, 1, 1, fp))
			{
				ts[0] = ts[1];
				ts[1] = ts[2];
				ts[2] = ts[3];
				ts[3] = *(ptr + size);
				size++;
				if ((ts[0] == 0x00) && (ts[1] == 0x00) && (ts[2] == 0x00) && (ts[3] == 0x01))
				{
					size -= 4;
					fseek(fp, -4, SEEK_CUR);
					qDebug() << "************===: read one frame end";
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	return size;
}

