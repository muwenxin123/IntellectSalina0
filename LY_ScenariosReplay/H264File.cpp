#include "H264File.h"
#include <QDebug>
H264File::H264File(QObject *parent, int buf_size)
	: QObject(parent),
	m_buf_size(buf_size)
{
	m_buf = new char[m_buf_size];
}
H264File::H264File() {
	m_buf_size = 50000;
	m_buf = new char[m_buf_size];
}

H264File::~H264File()
{
	delete[] m_buf;
}

bool H264File::Open(const char *path)
{
	m_file = fopen(path, "rb");
	int errNum = 0;
	if (m_file == NULL)
	{
		errNum = errno;
		qDebug() << "file faile===:" << errNum << strerror(errNum);
		return false;
	}

	return true;
}

void H264File::Close()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = NULL;
		m_count = 0;
		m_bytes_used = 0;
	}
}

int H264File::ReadFrame(char *in_buf, int in_buf_size, bool *end)
{
	if (m_file == NULL)
	{
		return -1;
	}

	int bytes_read = (int)fread(m_buf, 1, m_buf_size, m_file);
	if (bytes_read == 0)
	{
		fseek(m_file, 0, SEEK_SET);
		m_count = 0;
		m_bytes_used = 0;
		bytes_read = (int)fread(m_buf, 1, m_buf_size, m_file);
		if (bytes_read == 0)
		{
			this->Close();
			return -1;
		}
	}

	bool is_find_start = false, is_find_end = false;
	int i = 0, start_code = 3;
	*end = false;

	for (i = 0; i < bytes_read - 5; i++)
	{
		if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 1)
		{
			start_code = 3;
		}
		else if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 0 && m_buf[i + 3] == 1)
		{
			start_code = 4;
		}
		else
		{
			continue;
		}

		{
			is_find_start = true;
			i += 4;
			break;
		}
	}

	for (; i < bytes_read - 5; i++)
	{
		if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 1)
		{
			start_code = 3;
		}
		else if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 0 && m_buf[i + 3] == 1)
		{
			start_code = 4;
		}
		else
		{
			continue;
		}

		{
			is_find_end = true;
			break;
		}
	}

	bool flag = false;
	if (is_find_start && !is_find_end && m_count > 0)
	{
		flag = is_find_end = true;
		i = bytes_read;
		*end = true;
	}

	if (!is_find_start || !is_find_end)
	{
		this->Close();
		return -1;
	}

	int size = (i <= in_buf_size ? i : in_buf_size);
	memcpy(in_buf, m_buf, size);

	if (!flag)
	{
		m_count += 1;
		m_bytes_used += i;
	}
	else
	{
		m_count = 0;
		m_bytes_used = 0;
	}

	fseek(m_file, m_bytes_used, SEEK_SET);
	return size;
}

int read_one_frame(FILE *fp, unsigned char*ptr)
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
			if (fread(ptr+size, 1, 1, fp))
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

