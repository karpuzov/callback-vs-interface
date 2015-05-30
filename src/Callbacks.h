
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




void ReceiverB(int b)
{
	checkFinish(b);
}

void ReceiverC(std::shared_ptr<int> c)
{
	checkFinish(*c);
}




class CallbackGenerator
{
public:
	CallbackGenerator(std::function<void(int)> b, std::function<void(std::shared_ptr<int>)> c)
		: dummyWork(ioServive)
		, callbackB(b)
		, callbackC(c)
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
		ioServive.post([=](){ callbackB(b); });
	}
	void makeC(std::shared_ptr<int> c)
	{
		ioServive.post([=](){ callbackC(c); });
	}

private:
	boost::asio::io_service ioServive;
	boost::asio::io_service::work dummyWork;

	std::function<void(int)> callbackB;
	std::function<void(std::shared_ptr<int>)> callbackC;
};



