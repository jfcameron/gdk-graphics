// © Joseph Cameron - All Rights Reserved

#ifndef JFC_EVENTS_SMART_EVENT_H
#define JFC_EVENTS_SMART_EVENT_H

#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace jfc::events {
    /// \brief event class that automatically cleans up unused observers
    template<class... event_parameter_types_param>
    class smart_event final {
    public:
        using observer_type = std::function<void(event_parameter_types_param...)>;
        using observer_weak_ptr_type = std::weak_ptr<observer_type>;
        using observer_shared_ptr_type = std::shared_ptr<observer_type>;

        /// \brief adds an observer
        [[nodiscard]] observer_shared_ptr_type subscribe(observer_type aObserverFunctor) {
            auto pObserver = std::make_shared<observer_type>(std::move(aObserverFunctor));

            std::lock_guard<std::mutex> lock(m_ObserversMutex);

            m_Observers.push_back(pObserver);

            return pObserver;
        }

        /// \brief notify observers & remove any that have fallen out of scope
        void notify(const event_parameter_types_param&... params) const {
            std::vector<observer_shared_ptr_type> alive;

            {
                std::lock_guard<std::mutex> lock(m_ObserversMutex);

                m_Observers.erase(std::remove_if(m_Observers.begin(), m_Observers.end(),
                    [&alive](const observer_weak_ptr_type &each) {
                        auto pObserver = each.lock();

                        if (!pObserver) return true;

                        alive.push_back(std::move(pObserver));

                        return false;
                    }), m_Observers.end());
            }

            for (const auto &pObserver : alive) (*pObserver)(params...);
        }

        smart_event() = default;
        ~smart_event() = default;

    private:
        mutable std::vector<observer_weak_ptr_type> m_Observers;
        mutable std::mutex m_ObserversMutex;
    };
}

#endif
