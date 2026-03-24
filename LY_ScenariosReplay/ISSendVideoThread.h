#pragma once

#include <QObject>
#include <QThread>
#include "H264File.h"
#include  <vector>
#include  <set>
#include <QMultiMap>
#include <map>
#include <unordered_map>
#include "IScenariosReplayThread.h"

using namespace std;

class ISSendVideoThread : public QThread
{
	Q_OBJECT

public:
	ISSendVideoThread();
	~ISSendVideoThread();

	void pause();

	void stop();

	void setVideoPath(const set<LogDataInfo> dataArray, const int port, const int type);

	unordered_multimap<int, QString> videoPathMap;

	void closeFile();
private:

	bool                       is_runing;
	H264File                 h264File;
	QString                  q_path;

	int playType = 0;

	int read_one_frame(FILE *fp, unsigned char*ptr);

protected:

	void run();
};
