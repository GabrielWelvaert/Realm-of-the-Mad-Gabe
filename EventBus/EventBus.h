#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <map>
#include <typeindex>
#include <memory>
#include <list>
#include "Event.h"
#include <functional>

class IEventCallback{
    private:
        virtual void Call(Event& e) = 0; //pure virtual function
    public:
        inline virtual ~IEventCallback() = default;
        inline void Execute(Event& e){
            Call(e);
        }
};

//wrapper class around function ptr
template <typename TOwner, typename TEvent>
class EventCallback: public IEventCallback {
    private:
        // TCallbackFunction which is a pointer to a member function in the TOwner class, where
        // TEvent is the parameter list that will be used to call the correct member function

        typedef void (TOwner::*CallbackFunction)(TEvent&); //CallbackFunction type

        TOwner * ownerInstance;
        CallbackFunction callbackFunction;

        virtual void Call(Event& e) override {
            std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e)); //invoke function pointer
        }
    public:
        EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction){
            this->ownerInstance = ownerInstance;
            this->callbackFunction = callbackFunction;
        }

        virtual ~EventCallback() override = default;
    
};

typedef std::list<std::unique_ptr<IEventCallback>>HandlerList;

class EventBus{
    private:
        //key = typeindex for event : value = function pointers
        std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

    public:
        inline EventBus(){};
        inline ~EventBus(){};

        inline void Reset(){ //clear subscribers 
            subscribers.clear();
        }

        // subscribe to event of type <T>
        // ex: eventBus->SubscribeToEvent<CollisionEvent>(&Game::onCollision);
        template <typename TEvent, typename TOwner>
        void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&)){
            if (!subscribers[typeid(TEvent)].get()) {
                subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
            }
            auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
            subscribers[typeid(TEvent)]->push_back(std::move(subscriber)); //std::move is used to change owne;rship of unique_ptrs
        }
        // emit an event of type <T>
        // execute listener callbacks as soon as event occurs
        // ex: eventBus->EmitEvent<CollisionEvent>(player, enemy);
        template <typename TEvent, typename ...TArgs> 
        void EmitEvent(TArgs&& ...args){
            auto handlers = subscribers[typeid(TEvent)].get();
            if (handlers) {
                TEvent event(std::forward<TArgs>(args)...);
                for(auto it = handlers->begin(); it != handlers->end(); it++){
                    auto handler = it->get();
                    // TEvent event(std::forward<TArgs>(args)...); //moved above due to lvalue thing
                    handler->Execute(event);
                }
            }
        }
};

#endif