// © Joseph Cameron - All Rights Reserved

#ifndef JFC_EVENTS_EVENT_H
#define JFC_EVENTS_EVENT_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <mutex>
#include <utility>
#include <vector>

namespace jfc::events {
    /// \brief event class that requires the user to clean up after themselves
    template<class... event_parameter_types_param>
    class event final {
    public:
        using observer_type = std::function<void(event_parameter_types_param...)>;

        using token_type = std::size_t;

        /// \brief adds an observer, and returns the token that removes it
        token_type subscribe(observer_type observer) {
            std::lock_guard<std::mutex> lock(m_ObserversMutex);

            const auto token = m_NextToken++;

            m_Observers.emplace_back(token, std::move(observer));

            return token;
        }

        /// \brief removes an observer using a token. no effect if the observer doesnt exist
        void unsubscribe(const token_type token) {
            std::lock_guard<std::mutex> lock(m_ObserversMutex);

            const auto it = std::find_if(m_Observers.begin(), m_Observers.end(),
                [token](const auto &registered) { return registered.first == token; });

            if (it != m_Observers.end()) m_Observers.erase(it);
        }

        /// \brief notify observers
        void notify(event_parameter_types_param... params) const {
            std::vector<std::pair<token_type, observer_type>> observers;

            {
                std::lock_guard<std::mutex> lock(m_ObserversMutex);

                observers = m_Observers;
            }

            for (const auto &[token, observer] : observers) observer(params...);
        }

    private:
        std::vector<std::pair<token_type, observer_type>> m_Observers;
        token_type m_NextToken = 0;
        mutable std::mutex m_ObserversMutex;
    };
}

#endif
