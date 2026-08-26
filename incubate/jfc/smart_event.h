// © Joseph Cameron - All Rights Reserved

#ifndef JFC_SMART_EVENT_H
#define JFC_SMART_EVENT_H

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_set>

namespace jfc {
    /// \brief event class that automatically cleans up unused observers
    ///
    template<class... event_parameter_types_param>
    class smart_event final {
    public:
        using observer_type = std::function<void(event_parameter_types_param...)>;
        using observer_weak_ptr_type = std::weak_ptr<observer_type>;
        using observer_shared_ptr_type = std::shared_ptr<observer_type>;

        /// \brief adds an observer
        [[nodiscard]] observer_shared_ptr_type subscribe(observer_type aObserverFunctor) {
            auto pObserver = std::make_shared<observer_type>(aObserverFunctor);
            m_Observers.insert(pObserver);
            return pObserver;
        }

        /// \brief notify observers & remove any null observers
        void notify(const event_parameter_types_param&... params) const {
            std::lock_guard<std::mutex> lock(m_ObserversMutex);
            for (auto iter = m_Observers.begin(); iter != m_Observers.end(); ) {
                if (auto observer = iter->lock()) {
                    (*observer)(params...);
                    ++iter;
                } 
                else {
                    iter = m_Observers.erase(iter);  
                }
            }
        }
        
        smart_event() = default;
        ~smart_event() = default;

    private:
        struct observer_weak_ptr_hasher_method {
            std::size_t operator()(const std::weak_ptr<observer_type>& ptr) const {
                if (auto sp = ptr.lock()) {
                    return std::hash<std::shared_ptr<observer_type>>{}(sp);
                }
                return {};
            }
        };

        struct observer_weak_ptr_comparator_method {
            bool operator()(const std::weak_ptr<observer_type>& lhs, const std::weak_ptr<observer_type>& rhs) const {
                return !lhs.owner_before(rhs) && !rhs.owner_before(lhs);
            }
        };
        mutable std::unordered_set<observer_weak_ptr_type, observer_weak_ptr_hasher_method, observer_weak_ptr_comparator_method> m_Observers; 
        mutable std::mutex m_ObserversMutex;
    };
}

#endif

