#pragma once

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <exception>
#include <string>
#include <any>

// move event in 
// deletions
// ID check
namespace Events {

	class EventNotRegisteredException : public std::exception
	{
	public:
		EventNotRegisteredException(const std::string& id)
		{
			static const std::string prefix{ "The event with id " };
			static const std::string suffix{ " was not registered with the EventsRegistrar." };
			m_what = { prefix + id + suffix };
		}

		~EventNotRegisteredException() noexcept = default;

		virtual const char* what() const noexcept override
		{
			return m_what.c_str();
		}

	private:
		std::string m_what;
	};

	template<typename ...TEventArgs>
	class Event
	{
	public:
		using TCallbackFunction = std::function<void(TEventArgs...)>;

		void RegisterCallback(TCallbackFunction callback)
		{
			m_callbacks.push_back(callback);
		}

		template<typename ...TEventArgs>
		void Raise(TEventArgs... args)
		{
			for (const auto& callback : m_callbacks)
			{
				callback(args...);
			}
		}

	private:
		std::vector<TCallbackFunction> m_callbacks;
	};

	template<typename ...TEventArgs>
	class EventManager
	{
	public:

		using TCallbackFunction = typename Event<TEventArgs...>::TCallbackFunction;

		void RegisterEvent(const std::string& id, Event<TEventArgs...> evt)
		{
			m_events.insert({ id, evt });
		}

		void RegisterCallback(const std::string& id, TCallbackFunction callback)
		{
			FindEvent(id).RegisterCallback(callback);
		}

		template<typename ...TEventArgs>
		void RaiseEvent(const std::string& id, TEventArgs... args)
		{
			FindEvent(id).Raise(args...);
		}

	private:
		std::unordered_map<std::string, Event<TEventArgs...>> m_events;

		Event<TEventArgs...>& FindEvent(const std::string& id) {
			const auto& eventIterator{ m_events.find(id) };
			if (eventIterator != m_events.end()) {
				return eventIterator->second;
			}
			throw EventNotRegisteredException(id);
		}
	};

	class EventsAggregator
	{
	public:

		template<typename ...TEventArgs>
		void RegisterEvent(const std::string& id)
		{
			Event<TEventArgs...> event{};
			GetEventManager<TEventArgs...>(id).RegisterEvent(id, event);
		}

		template<typename ...TEventArgs>
		void RegisterCallback(const std::string& id, std::function<void(TEventArgs...)> callback)
		{
			GetEventManager<TEventArgs...>(id).RegisterCallback(id, callback);
		}

		template<typename ...TEventArgs>
		void RaiseEvent(const std::string& id, TEventArgs... args)
		{
			GetEventManager<TEventArgs...>(id).RaiseEvent(id, args...);
		}

	private:
		template<typename ...TEventArgs>
		EventManager<TEventArgs...>& GetEventManager(const std::string& id)
		{
			const auto eventManagerIterator{ eventManagers.find(id) };
			if (eventManagerIterator == eventManagers.end())
			{
				EventManager<TEventArgs...> manager{};
				eventManagers.insert({ id, manager });
			}
			return std::any_cast<EventManager<TEventArgs...>&>(eventManagers[id]);
		}

		std::unordered_map<std::string, std::any> eventManagers;
	};


}
