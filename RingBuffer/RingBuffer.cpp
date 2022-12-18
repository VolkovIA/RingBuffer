#include <iostream>

// ���������� ���������� ������ � ������� ������������ �������,
// � ������� ������������� ������� � � ������� ����������� ������.

// ���������� �� ����������� ��������
template<class T, int size = 10>
class RingBuf
{
public:
	RingBuf() {};
	~RingBuf() {};

	int tryWrite(T val)
	{
		if (write + 1 == read)
		{
			return static_cast<int>(RBReturns::RINGBUFFER_ERR_FULL);
		}

		buf[write] = val;
		++write;
		if (write == size)
		{
			write = 0;
		}

		return static_cast<int>(RBReturns::RINGBUFFER_OK);
	}

	int tryRead(T* val)
	{
		if (read == write)
		{
			return static_cast<int>(RBReturns::RINGBUFFER_ERR_EMPTY);
		}

		*val = buf[read];
		++read;

		if (read >= size)
		{
			read = 0;
		}

		return static_cast<int>(RBReturns::RINGBUFFER_OK);
	}

	int getSize()
	{
		return size;
	}

private:
	T buf[size];
	unsigned int write = 0;
	unsigned int read = 0;
};

// ���������� � ������������ ��������
template<class T>
class DynamicRingBuf
{
public:
	DynamicRingBuf(unsigned int size = 10) : sizeBuf{ size }
	{
		buf = new T[sizeBuf];
	};
	~DynamicRingBuf()
	{
		delete[] buf;
	};

	int tryWrite(T val)
	{
		if (write + 1 == read)
		{
			return static_cast<int>(RBReturns::RINGBUFFER_ERR_FULL);
		}

		buf[write] = val;
		++write;
		if (write == sizeBuf)
		{
			write = 0;
		}

		return static_cast<int>(RBReturns::RINGBUFFER_OK);
	}

	int tryRead(T* val)
	{
		if (read == write)
		{
			return static_cast<int>(RBReturns::RINGBUFFER_ERR_EMPTY);
		}

		*val = buf[read];
		++read;

		if (read >= sizeBuf)
		{
			read = 0;
		}

		return static_cast<int>(RBReturns::RINGBUFFER_OK);
	}

	void createBuf(int size)
	{
		if (buf == nullptr)
		{
			sizeBuf = size;
			buf = new T[sizeBuf];
			write = 0;
			read = 0;
		}
	}

	void eraseBuf()
	{
		if (buf)
		{
			delete[] buf;
			buf = nullptr;
		}
	}

	unsigned int getSize()
	{
		return sizeBuf;
	}

private:
	T* buf;
	unsigned int sizeBuf = 0;
	unsigned int write = 0;
	unsigned int read = 0;
};

// ���������� � ����������� �������

// ��������� ��� �������� ��������
enum class RBReturns : int
{
	RINGBUFFER_OK = 0,
	RINGBUFFER_ERR_FULL = -1,
	RINGBUFFER_ERR_EMPTY = -2,
	TRY_READ_ERROR = -3
};

template <class T>
struct Element // ������� ������
{
	T data;
	
	// ��������� �� ���������� ������� � �� ���������
	Element* next, * prev;
};

template <class T>
class ListRingBuf // ��������� �����
{
public:
	ListRingBuf() : write{ nullptr }, read{ nullptr }, sizeBuf{ 0 } {};
	
	//������� ����� ������� ������� � ����������������
	ListRingBuf(unsigned int size) : write{ nullptr }, read{ nullptr }, sizeBuf{ 0 }
	{
		for (sizeBuf; sizeBuf < size; ++sizeBuf)
		{
			// ��������� ������ ��� ������ �������� ������
			Element<T>* newElem = new Element<T>;
			newElem->data = 0;
			// ���� �������� ������ ������� ������, �� ��������� next � prev ������ ��������� �� ��� �������
			if (write == nullptr)
			{
				write = newElem;
				newElem->next = newElem;
				newElem->prev = newElem;
			}
			else // ���� �������� ����� ����������� ������� ������, �� ��������� next � prev ��������� �� ��������� � ���������� �������� ������
			{
				newElem->next = write->next;
				newElem->prev = write;
				write->next->prev = newElem;
				write->next = newElem;
			}
		}
		read = write;
	}
	~ListRingBuf()
	{
		if (write == nullptr)
		{
			return;
		}
		if (write)
		{
			Element<T>* tmp;

			for (sizeBuf; sizeBuf > 0; --sizeBuf)
			{
				tmp = write->next;
				delete write;
				write = tmp;
			}
		}
	};

	void createBuf(unsigned int size)
	{
		for (sizeBuf; sizeBuf < size; ++sizeBuf)
		{
			// ��������� ������ ��� ������ �������� ������
			Element<T>* newElem = new Element<T>;
			newElem->data = 0;
			if (write == nullptr)
			{
				write = newElem;
				newElem->next = newElem;
				newElem->prev = newElem;
				return;
			}
			newElem->next = write->next;
			newElem->prev = write;
			write->next->prev = newElem;
			write->next = newElem;
		}
		read = write;
	}
	
	// ������� ������
	void eraseBuf()
	{
		if (write == nullptr)
		{
			return;
		}
		if (write)
		{
			Element<T>* tmp;

			for (sizeBuf; sizeBuf > 0; --sizeBuf)
			{
				tmp = write->next;
				delete write;
				write = tmp;
			}

			read = write = nullptr;
		}
	}
	
	// ��������� �������� ����� ��������
	void pushNext(T val)
	{
		Element<T>* newElem = new Element<T>;
		newElem->data = val;
		if (write == nullptr)
		{
			write = newElem;
			newElem->next = newElem;
			newElem->prev = newElem;
			++sizeBuf;
			return;
		}

		newElem->next = write->next;
		newElem->prev = write;
		write->next->prev = newElem;
		write->next = newElem;
		++sizeBuf;
	}
	
	// ��������� �������� ����� �������
	void pushPrev(T val)
	{
		Element<T>* newElem = new Element<T>;
		newElem->data = val;
		if (write == nullptr)
		{
			write = newElem;
			newElem->next = newElem;
			newElem->prev = newElem;
			++sizeBuf;
			return;
		}

		newElem->next = write;
		newElem->prev = write->prev;
		write->prev->next = newElem;
		write->prev = newElem;
		++sizeBuf;
	}

	// ���������� �������� T& val � data
	int tryWrite(T val)
	{
		if (write->next == read)
		{
			return static_cast<int>(RBReturns::RINGBUFFER_ERR_FULL);
		}

		write->data = val;
		write = write->next;

		return static_cast<int>(RBReturns::RINGBUFFER_OK);
	}

	// ���������� ����������� data � ���������� T* val
	int tryRead(T* val)
	{
		
		if (val < 0)
		{
			return static_cast<int>(RBReturns::TRY_READ_ERROR;
		}
		if (read == write)
		{
			return static_cast<int>(RBReturns::RINGBUFFER_ERR_EMPTY);
		}

		std::cout << read->data;
		*val = read->data;
		read = read->next;

		return static_cast<int>(RBReturns::RINGBUFFER_OK);
	}

	unsigned int getSizeBuf() const
	{
		return sizeBuf;
	}

private:
	Element<T>* read, * write;
	unsigned int sizeBuf;
};

int main()
{
	int Read = 0;
	ListRingBuf<int> RingBuf(10);
	RingBuf.tryWrite(99);
	/*RingBuf.tryWrite(56);
	RingBuf.tryRead(Read);
	++Read;
	RingBuf.tryRead(Read);*/

	return 0;
}