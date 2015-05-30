
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/timer/timer.hpp>
#include <thread>
#include <map>
#include <mutex>


extern boost::timer::cpu_timer Timer;
extern boost::timer::cpu_times ElapsedTime;
extern std::mutex Mutex;

void checkFinish(int x);


class Listener
{
public:
	virtual void receiveB(int b) = 0;
	virtual void receiveC(std::shared_ptr<int> c) = 0;

	~Listener() {}
};


class ConcreteListener
	: public Listener
{
public:
	void receiveB(int b) override
	{
		checkFinish(b);
	}
	void receiveC(std::shared_ptr<int> c) override
	{
		checkFinish(*c);
	}
};



class AsyncGenerator
{
public:
	AsyncGenerator(std::shared_ptr<Listener> listener)
		: receiver(listener)
		, dummyWork(ioServive)
	{
		std::thread t(
			[](boost::asio::io_service& io)
			{
				Mutex.lock();
				io.run();
			},
			std::ref(ioServive)
		);
		t.detach();
	}

	void makeB(int b)
	{
		ioServive.post([=](){ receiver->receiveB(b); });
	}
	void makeC(std::shared_ptr<int> c)
	{
		ioServive.post([=](){ receiver->receiveC(c); });
	}

private:
	std::shared_ptr<Listener> receiver;
	boost::asio::io_service ioServive;
	boost::asio::io_service::work dummyWork;
};
