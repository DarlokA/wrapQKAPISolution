#pragma once
namespace ___CritSecLocker___
{
	template <typename TLocker>
	class CLocker
	{
		typedef typename TLocker Locker;
	public:
		explicit CLocker(Locker& cs)
			:m_cs(cs)
		{
			m_cs.Lock();
		}
		~CLocker()
		{
			m_cs.Unlock();
		}
	private:
		Locker& m_cs;
	};
}

#define CritSecMethod( cs )  ___CritSecLocker___::CLocker< ATL::CComAutoCriticalSection >locker(cs)