#ifndef CONFIG_DYNAMIC_H_
#define CONFIG_DYNAMIC_H_

enum FollowModel {
    RANDOM_FOLLOW,
    PREFERENTIAL_FOLLOW,
    ENTITY_FOLLOW,
    PREFERENTIAL_ENTITY_FOLLOW
};

#include <cmath>
#include <vector>
#include "network.h"

#include "tweets.h"

// The relevance table for an entity preference class
// determines how likely they are to act on a given
// tweet (whether a retweet or not) that they see.

struct DistanceFuncTable {
    std::vector<TweetRateVec> rates;
};

struct HumourFuncTable {
    std::vector<DistanceFuncTable> subtable;
};

struct EntityTypeFuncTable {
    std::vector<HumourFuncTable> subtable;
};

struct EntityPreferenceClass {
    std::string name;
    // This is created from a Python function in INFILE.py
    EntityTypeFuncTable relevance_func_table;
};

// Also referred to as the Omega function, see INFILE.yaml for details:
struct TweetObservationPDF {
    double initial_resolution;
    std::vector<double> values;

    // Inverse of our exponential function:
    int get_probability_bin(double time) {
        const double log2 = 0.30102999566;
        double R = initial_resolution;
        double L = log( (R+time) / (3.0 * R));
        int bin = (int)ceil((L - log2) / log2);
        if (bin >= values.size()) {
            return values.size() - 1;
        }
        return bin;
    }
};

// Config variables, read from INFILE.yaml
struct ParsedConfig {
    // 'analysis' config options
    int initial_entities, max_entities;
    double max_time;
    bool use_random_time_increment;
    bool use_preferential_follow;
    bool use_flawed_followback;
    bool use_barabasi;
    bool entity_stats;
    FollowModel follow_model;
    FollowerSetRates follower_rates;

    // 'rates' config options
    double rate_add;
    double unfollow_tweet_rate;

    // 'output' config options
    bool output_stdout_basic, output_stdout_summary;
    bool output_visualize;
    bool degree_distributions;
    bool output_tweet_analysis;
    bool output_verbose;
    // 'X_category' config options

    // Thresholds are filled, entity lists empty
    CategoryGrouper tweet_ranks, follow_ranks;
    CategoryGrouper retweet_ranks;
    LanguageProbabilities lang_probs;

    std::vector<double> follow_probabilities;
    // 'entities' config options
    // Note: Weights are filled, entity lists empty
    EntityTypeVector entity_types;
    Add_Rates add_rates;

    std::vector<EntityPreferenceClass> pref_classes;

    // command-line config options
    bool handle_ctrlc;

    // Tweet relevance functions
    TweetObservationPDF tweet_obs;

    /* Most config values are optional -- place defaults here. */
    ParsedConfig() {
        initial_entities = 0;
        max_entities = 1000;
        max_time = INFINITY;

        entity_stats = false;
        use_random_time_increment = false;
        use_preferential_follow = false;
        use_barabasi = false;
        use_flawed_followback = false;
        output_verbose = false;

        unfollow_tweet_rate = 0.0;
        follow_model = RANDOM_FOLLOW;

        rate_add = 0;

        output_stdout_basic = output_stdout_summary = true;
        output_visualize = false;
        output_tweet_analysis = false;
		degree_distributions = true;
		handle_ctrlc = false;
    }
};

ParsedConfig parse_yaml_configuration(const char* file_name);

#endif
