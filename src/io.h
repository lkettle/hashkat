#ifndef __IO_H_
#define __IO_H_

#include <fstream>

#include "analyzer.h"
#include "network.h"


void output_position(Network& network, int end_time);

void output_network_statistics(AnalysisState& state);

int factorial(int input_number);
void Categories_Check(CategoryGrouper& tweeting, CategoryGrouper& following, CategoryGrouper& retweeting);
void entity_statistics(Network& network,int n_follows, int n_entities, int max_entities, EntityType* entitytype);
void tweets_distribution(Network& network, int n_users);
int rand_int(int max);
void degree_distributions(Network& network);
bool quick_rate_check(EntityTypeVector& ets, double& correct_val, int& i, int& j);
bool entity_checks(EntityTypeVector& ets, Network& network, AnalysisState& state, double& rate_add, int& initial_entities);
void whos_following_who(EntityTypeVector& ets, Network& network);

#endif