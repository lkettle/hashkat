#ifndef TIMECATGROUPER_H_
#define TIMECATGROUPER_H_

#include "config_static.h"

#include "network.h"
#include "CatGrouper.h"


struct TimeCatConfig {
    static int get_category(Network& N, EntityID entity) {
        return (N[entity].creation_time / TIME_CAT_FREQ);
    }
};

typedef CatGrouper<TimeCatConfig> TimeCatGrouper;

#endif
