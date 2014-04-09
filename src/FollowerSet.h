#ifndef FOLLOWERSET_H_
#define FOLLOWERSET_H_

#include <cstdio>
#include <cmath>
#include "util.h"

#include "util/FlexibleSet.h"

// For bin limits:
#include "config_static.h"

// Forward declarations to prevent circular imports:
struct Entity;
struct Tweet;
struct TweetContent;

/*****************************************************************************
 * Categorization layers of the follower set.
 * The categorization layers are:
 *   Language
 *   X Preference class
 *   X Ideology
 *
 * The weight determiner layers are:
 *   Entity preference class
 *   X Tweet type (for ideological tweets, whether ideologies match)
 *   X Original tweeter entity type
 *****************************************************************************/

// Leaf layer
struct IdeologyLayer {
    static const int N_SUBLAYERS = N_BIN_IDEOLOGIES;

    struct Weights {
        double weights[N_SUBLAYERS] = {0};
    };

    static int classify(Entity& entity);

    int n_elems = 0; // Total
    FlexibleSet<int> sublayers[N_SUBLAYERS];
};

struct RegionLayer {
    typedef IdeologyLayer ChildLayer;
    static const int N_SUBLAYERS = N_BIN_REGIONS;

    struct Weights {
        ChildLayer::Weights subweights[N_SUBLAYERS];
        double weights[N_SUBLAYERS] = {0};
    };

    static int classify(Entity& entity);

    int n_elems = 0; // Total
    ChildLayer sublayers[N_SUBLAYERS];
};

struct PreferenceClassLayer {
    typedef RegionLayer ChildLayer;
    static const int N_SUBLAYERS = N_BIN_PREFERENCE_CLASS;

    struct Weights {
        ChildLayer::Weights subweights[N_SUBLAYERS];
        double weights[N_SUBLAYERS] = {0};
    };

    static int classify(Entity& entity);

    int n_elems = 0; // Total
    ChildLayer sublayers[N_SUBLAYERS];
};

// Top layer
struct LanguageLayer {
    typedef PreferenceClassLayer ChildLayer;
    static const int N_SUBLAYERS = N_LANGS;

    struct Weights {
        ChildLayer::Weights subweights[N_SUBLAYERS];
        double weights[N_SUBLAYERS] = {0};
    };

    static int classify(Entity& entity);

    int n_elems = 0; // Total
    ChildLayer sublayers[N_SUBLAYERS];
};

/*****************************************************************************
 * Implementation of the follower set:
 *****************************************************************************/

struct FollowerSet {
    typedef LanguageLayer TopLayer;
    typedef TopLayer::Weights Weights;

    // Weights for determining whether a tweet has a reaction (follow/retweet).
    // Note that this is primarily decided by a tweet type, observer preference class,
    // and author's entity type. Note that for TWEET_IDEOLOGICAL, we resolve first whether it should be
    // TWEET_IDEOLOGICAL_DIFFERENT, otherwise tweet ideology is matching.
    // Language layer does not need its own weight determiner layer,
    // it is a hard all-or-nothing cutoff for tweet reaction.
    struct WeightDeterminer {
        double weights[N_BIN_PREFERENCE_CLASS][N_TWEET_TYPES][N_BIN_ENTITY_TYPES];
        WeightDeterminer() {
            memset(weights, 0, sizeof(double) * N_BIN_PREFERENCE_CLASS * N_TWEET_TYPES * N_BIN_ENTITY_TYPES);
        }

        double get_weight(Entity& author, TweetContent& content);
    };

    std::vector<int> as_vector();

    /* Returns false if the element already existed */
    bool add(Entity& entity);

    /* Returns true if the element already existed */
    bool remove(Entity& entity);

    /* Returns an element, provided the given weights */
    bool pick_random_weighted(MTwist rng, Weights& weights, int& id);

    /* Returns an element, weighing all options equally */
    bool pick_random_uniform(MTwist& rng, int& id);

    void print();

    size_t size() const {
        return followers.n_elems;
    }

    void determine_tweet_weights(Entity& author, TweetContent& content, WeightDeterminer& determiner, /*Weights placed here: */ Weights& output);

private:
    // Holds the actual followers:
    TopLayer followers;
};


#endif /* FOLLOWERSET_H_ */