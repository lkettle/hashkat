#ifndef ENTITY_H_
#define ENTITY_H_

#include "CircularBuffer.h"
#include "MemPoolVector.h"

typedef MemPoolVector<int, FOLLOW_LIST_THRESHOLD1> FollowList;
typedef MemPoolVector<int, FOLLOWER_LIST_THRESHOLD1> FollowerList;

struct Retweet {
    int original_tweeter;
    double time;
    Retweet() {
    }
    Retweet(int tweet, double time) :
            original_tweeter(tweet), time(time) {
    }
};

// Store the last 'RETWEETS_STORED' tweets, discarding old ones.
typedef CircularBuffer<Retweet, RETWEETS_STORED> RetweetBuffer;

struct Entity {
    int entity;
    int n_tweets, n_retweets;
    double creation_time;
    float x_location, y_location;

    // Store the two directions of the follow relationship
    FollowList follow_set;
    FollowerList follower_set;

    RetweetBuffer retweets;

    Entity() {
        entity = 0;
        creation_time = 0.0;
        x_location = -1, y_location = -1;
        n_tweets = 0;
        n_retweets = 0;
    }
};

struct EntityID {
    int slot_id;
    /* The amount of times this slot has been reused. */
    int slot_iter;
    explicit EntityID(int slot_id = -1, int slot_iter = -1) {
        this->slot_id = slot_id;
        this->slot_iter = slot_iter;
    }

    bool operator==(const EntityID& o) const {
        return slot_id == o.slot_id && slot_iter == o.slot_iter;
    }
};

#endif
