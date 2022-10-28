#pragma once

#include <list>
#include <typeindex>
#include <map>
#include "../Logger/Logger.h"
#include "Event.hpp"

/**
 * @brief BaseEventCallBack
 */
class BaseEventCallBack {
public:
    virtual ~BaseEventCallBack() = default;

    void Execute(Event& e) {
        Call(e);
    }

private:
    virtual void Call(Event& e) = 0;

};

/**
 * @brief EventCallBack
 * @tparam TOwner
 * @tparam TEvent
 */
template<typename TOwner, typename TEvent>
class EventCallBack : public BaseEventCallBack {
public:
    typedef void (TOwner::*CallBackFunction)(TEvent&);

    EventCallBack(TOwner* ownerInstance, CallBackFunction callBackFunction) :
            ownerInstance(ownerInstance),
            callBackFunction(callBackFunction) {}

    ~EventCallBack() override = default;

private:
    TOwner* ownerInstance;
    CallBackFunction callBackFunction;

    void Call(Event& e) override {
        std::invoke(callBackFunction, ownerInstance, static_cast<TEvent&>(e));
    }

};

/**
 * @brief EventBus
 */
class EventBus {
public:
    EventBus() {
        Logger::Log("EventBus ctor called!");
    }

    ~EventBus() {
        Logger::Log("EventBus dtor called!");
    }

    void Reset() {
        subscribers.clear();
    }

    /**
     * @brief Subscribe to an event type <TEvent>.
     * In our implementation, a listener subscribes to an event
     * Example: eventBus->SubscribeToEvent<CollisionEvent>(this, &Game::onCollision);
     * @tparam TOwner
     * @tparam TEvent
     * @param ownerInstance
     * @param CallBackFunction
     */
    template<typename TOwner, typename TEvent>
    void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*CallBackFunction)(TEvent&)) {
        if (!subscribers[typeid(TEvent)].get()) {
            subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
        }
        subscribers[typeid(TEvent)]->emplace_back(
                std::make_unique<EventCallBack<TOwner, TEvent>>(ownerInstance, CallBackFunction));
    }

    /**
     * @brief Emit an event of type <TEvent>.
     * In our implementation, as soon as something emits an event
     * we go ahead and execute all the listener callback functions
     * Example: eventBus->EmitEvent<CollisionEvent>(player, enemy);
     * @tparam TEvent
     * @tparam Args
     * @param args
     */
    template<typename TEvent, typename ...Args>
    void EmitEvent(Args&& ...args) {
        auto handlers = subscribers[typeid(TEvent)].get();
        if (handlers) {
            TEvent event{std::forward<Args>(args)...};
            for (auto& handler: *handlers) {
                handler->Execute(event);
            }
        }
    }

private:
    using HandlerList = std::list<std::unique_ptr<BaseEventCallBack>>;

    std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

};
