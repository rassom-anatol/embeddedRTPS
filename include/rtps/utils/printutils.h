//
// Created by andreas on 13.01.19.
//

#ifndef RTPS_PRINTUTILS_H
#define RTPS_PRINTUTILS_H

#include "rtps/common/types.h"

inline void printEntityId(rtps::EntityId_t id){
    for(const auto byte : id.entityKey){
        printf("%i", byte);
    }
    printf("%i", static_cast<uint8_t>(id.entityKind));
}

inline void printGuidPrefix(rtps::GuidPrefix_t prefix){
    for(const auto byte : prefix.id){
        printf("%i", byte);
    }
}

inline void printGuid(rtps::Guid guid){
    printGuidPrefix(guid.prefix);
    printf(":");
    printEntityId(guid.entityId);
}

#endif //RTPS_PRINTUTILS_H
