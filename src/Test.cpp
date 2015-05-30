
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/timer/timer.hpp>
//#include <thread>
#include <mutex>


#include "Interfaces.h"
#include "Callbacks.h"


boost::timer::cpu_timer Timer;
boost::timer::cpu_times ElapsedTime;

std::mutex Mutex;


void checkFinish(int x)
{
	if (x == 0)
	{
		ElapsedTime = Timer.elapsed();
		Mutex.unlock();
	}
}






int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "USAGE: <Interfaces|Callbacks>" << std::endl;
		return EXIT_SUCCESS;
	}

	auto c = std::make_shared<int>(3);
	const int ciclesB = 1000000 * 5;
	const int ciclesC = 1000000 * 5;

	if (std::string("interfaces") == argv[1])
	{
		auto listener = std::make_shared<ConcreteListener>();
		auto generator = std::make_shared<AsyncGenerator>(listener);

		Timer.start();

		for (int i = 0; i < ciclesB; ++i)
		{
			generator->makeB(1);
		}
		for (int i = 0; i < ciclesC; ++i)
		{
			generator->makeC(c);
		}
		generator->makeB(0);

		Mutex.lock();
	}
	else if (std::string("callbacks") == argv[1])
	{
		auto generator = std::make_shared<CallbackGenerator>(ReceiverB, ReceiverC);

		Timer.start();

		for (int i = 0; i < ciclesB; ++i)
		{
			generator->makeB(1);
		}
		for (int i = 0; i < ciclesC; ++i)
		{
			generator->makeC(c);
		}
		generator->makeB(0);

		Mutex.lock();
	}

	std::cout << "ElapsedTime: " << boost::timer::format(ElapsedTime) << std::endl;
	std::cin.get();
	return EXIT_SUCCESS;
}
