#pragma once
#include <string>
#include "Model.h"

namespace TimeManagement {
    class EventService {
    public:
        std::string generateNewEventId();
        bool saveEvent(const Event& event);
    };
}
