// Events.cpp : Defines the entry point for the application.
//

#include "flexivents.h"

#include <iostream>

using namespace Events;

void pl(const std::string& s) {
	std::cout << s << std::endl;
}


#define DefineEvent(EVENT_NAME, CALLBACK_INTERFACE_NAME, ...) \
	const std::string EVENT_NAME { #EVENT_NAME };    \
    using CALLBACK_INTERFACE_NAME = Event<__VA_ARGS__>::TCallbackFunction;

namespace MyEvents
{

	DefineEvent(VoidEvent, IVoidEventCallback, void);
	DefineEvent(BoolEvent, IBooleanEventCallback, bool);
	DefineEvent(MultiEvent, IMultiEventCallback, int, int);
}

Event<void>::TCallbackFunction voidCallback{
	[]() {
		pl("void event");
	}
};

std::function<void(bool)> boolCallback{
	[](bool b)
	{
		pl("bool event");
		std::cout << b << std::endl;
	}
};

std::function<void(int, int)> multiCallback{
	[](int a, int b) {
		pl("multi event");
		std::cout << a << " times " << b << " equals " << a * b << std::endl;
	}
};

int main()
{
	std::unique_ptr<EventsAggregator> ea{ std::make_unique<EventsAggregator>() };

	ea->RegisterEvent(MyEvents::VoidEvent);
	ea->RegisterEvent<bool>(MyEvents::BoolEvent);
	ea->RegisterEvent<int, int>(MyEvents::MultiEvent);

    ea->RegisterCallback(MyEvents::VoidEvent, voidCallback);
	ea->RegisterCallback<bool>(MyEvents::BoolEvent, boolCallback);
	ea->RegisterCallback<int, int>(MyEvents::MultiEvent, multiCallback);

	ea->RaiseEvent(MyEvents::BoolEvent, false);
	ea->RaiseEvent(MyEvents::BoolEvent, true);
	ea->RaiseEvent(MyEvents::MultiEvent, 6, 9);
	ea->RaiseEvent(MyEvents::VoidEvent);

	try {
		ea->RegisterCallback<bool>("TEST", boolCallback);
	}
	catch (const EventNotRegisteredException& e)
	{
		std::cout << e.what();
	}
	return 0;
}