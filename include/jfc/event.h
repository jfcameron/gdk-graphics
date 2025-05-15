// © Joseph Cameron - All Rights Reserved

#ifndef JFC_EVENT_H
#define JFC_EVENT_H

#include <algorithm>
#include <mutex>
#include <vector> 

namespace jfc {
    /// \brief event class that requires the user to clean up after themselves
    ///
    template<class... event_parameter_types_param>
    class event final {
    public:
        using observer_type = std::function<void(event_parameter_types_param...)>;

        /// \brief adds an observer
        void subscribe(observer_type observer) {
            m_Observers.push_back(std::move(observer));
        }

        /// \brief removes an observer
        void unsubscribe(const observer_type &observer) {
            auto it = std::find_if(m_Observers.begin(), m_Observers.end(),
                [&](const observer_type& registeredObserver) {
                    return registeredObserver.target_type() == observer.target_type();
                });

            if (it != m_Observers.end()) m_Observers.erase(it); 
        }

        /// \brief notify observers
        void notify(event_parameter_types_param... params) const {
            std::lock_guard<std::mutex> lock(m_ObserversMutex);
            for (const auto &observer : m_Observers) {
                observer(params...);
            }
        }

    private:
        std::vector<observer_type> m_Observers;
        mutable std::mutex m_ObserversMutex;
    };
}

#endif

