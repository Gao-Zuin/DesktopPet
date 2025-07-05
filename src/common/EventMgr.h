#ifndef __EVENT_MGR_H__
#define __EVENT_MGR_H__

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>
#include <type_traits>
#include <algorithm>
#include "Singleton.h"
// 前置声明
template <typename TEvent>
class EventListener;

class EventListenerBase {
public:
    virtual ~EventListenerBase() = default;
};

template <typename TEvent>
class EventListener : public EventListenerBase {
public:
    virtual void OnEvent(TEvent event) = 0;
};

class EventMgr : public Singleton<EventMgr> {
    friend class Singleton<EventMgr>;  // 允许Singleton访问私有构造函数
    
private:
    // 私有构造函数
    EventMgr() = default;
    // 事件分发器接口
    class IEventDispatcher {
    public:
        virtual ~IEventDispatcher() = default;
        virtual void Dispatch(const void* event) = 0;
        virtual void AddListener(EventListenerBase* listener) = 0;
        virtual void RemoveListener(EventListenerBase* listener) = 0;
    };

    // 具体事件类型的分发器
    template <typename TEvent>
    class EventDispatcher : public IEventDispatcher {
    public:
        void Dispatch(const void* event) override {
            auto concrete_event = static_cast<const TEvent*>(event);
            for (auto listener : listeners_) {
                static_cast<EventListener<TEvent>*>(listener)->OnEvent(*concrete_event);
            }
        }

        void AddListener(EventListenerBase* listener) override {
            listeners_.push_back(static_cast<EventListener<TEvent>*>(listener));
        }

        void RemoveListener(EventListenerBase* listener) override {
            auto it = std::find(listeners_.begin(), listeners_.end(), 
                               static_cast<EventListener<TEvent>*>(listener));
            if (it != listeners_.end()) {
                listeners_.erase(it);
            }
        }

    private:
        std::vector<EventListener<TEvent>*> listeners_;
    };

public:
    virtual ~EventMgr() noexcept = default;

    template <typename TEvent>
    void RegisterEvent(EventListener<TEvent>* listener) {
        static_assert(std::is_class<TEvent>::value, "TEvent must be a struct/class type");
        auto type = std::type_index(typeid(TEvent));
        auto& dispatcher = GetDispatcher<TEvent>(type);
        dispatcher.AddListener(listener);
    }

    template <typename TEvent>
    void SendEvent(TEvent event) {
        static_assert(std::is_class<TEvent>::value, "TEvent must be a struct/class type");
        auto type = std::type_index(typeid(TEvent));
        auto it = dispatchers_.find(type);
        if (it != dispatchers_.end()) {
            it->second->Dispatch(&event);
        }
    }

    template <typename TEvent>
    void UnregisterEvent(EventListener<TEvent>* listener) {
        static_assert(std::is_class<TEvent>::value, "TEvent must be a struct/class type");
        auto type = std::type_index(typeid(TEvent));
        auto it = dispatchers_.find(type);
        if (it != dispatchers_.end()) {
            it->second->RemoveListener(listener);
        }
    }

private:
    template <typename TEvent>
    IEventDispatcher& GetDispatcher(std::type_index type) {
        auto it = dispatchers_.find(type);
        if (it == dispatchers_.end()) {
            auto dispatcher = std::make_unique<EventDispatcher<TEvent>>();
            auto result = dispatchers_.emplace(type, std::move(dispatcher));
            return *result.first->second;
        }
        return *it->second;
    }

    std::unordered_map<std::type_index, std::unique_ptr<IEventDispatcher>> dispatchers_;
};

#endif // EVENT_MGR_H