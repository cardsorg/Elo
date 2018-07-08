/*
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#pragma once

#include <cmath>
#include <functional>
#include <utility>
#include <vector>

namespace Elo {

double WIN = 1;
double DRAW = 0.5;
double LOSS = 0;

double round_places(double x, double places) {
	if (x == 0) {
		return 0;
	}

	double scale = std::pow(10.0, places);
	return std::round(x * scale) / scale;
}

struct Distribution {
public:
	virtual double cdf(double x, double mean) const { return 0; };
	virtual ~Distribution() {};
};

struct LogisticDistribution : public Distribution {
	double base;
	double scale;

public:
	LogisticDistribution(double initial_base, double initial_scale):
		base(initial_base), scale(initial_scale) {};

	virtual double cdf(double x, double mean) const override {
		return 1.0 / (1.0 + std::pow(base, -((x - mean) / scale)));
	}
};

struct NormalDistribution : public Distribution {
	double stdev;

public:
	NormalDistribution(double initial_stdev):
		stdev(initial_stdev) {};

	virtual double cdf(double x, double mean) const override {
		return (1 + std::erf((x - mean) / (stdev * std::sqrt(2)))) / 2;
	}
};

LogisticDistribution default_distribution(10, 400);

class Player;
class Match;

struct Configuration {
	Distribution& dist;
	std::function<double(Player&)> calculate_k;

	Configuration(Distribution& initial_distribution, double initial_k):
		dist(initial_distribution), calculate_k([initial_k](Player& p) {return initial_k;}) {};

	Configuration(Distribution& initial_distribution, std::function<double(Player&)> initial_calculate_k):
		dist(initial_distribution), calculate_k(initial_calculate_k) {};
};

Configuration default_configuration(default_distribution, 32);

class Player {
	std::vector<Match> matches;

public:
	double rating;
	Configuration config;

	Player(double initial_rating, Configuration initial_configuration = default_configuration):
		rating(initial_rating), config(initial_configuration) {};

	double round_rating(double places) {
		return round_places(rating, places);
	}

	std::vector<Match> get_matches() {
		return matches;
	}

	// Add match WITHOUT changing rating.
	void add_match(Match& m) {
		matches.push_back(m);
	}
};

struct Match {
	Player& player_a;
	Player& player_b;
	// This is the result for player_a.
	double result;

	Match(Player& initial_player_a, Player& initial_player_b, double initial_result):
		player_a(initial_player_a), player_b(initial_player_b), result(initial_result) {};

	void apply() {
		double player_a_delta = player_a.config.calculate_k(player_a) * (result - player_a.config.dist.cdf(player_b.rating, player_a.rating));
		double player_b_delta = player_b.config.calculate_k(player_b) * ((1 - result) - player_b.config.dist.cdf(player_a.rating, player_b.rating));
		player_a.rating += player_a_delta;
		player_b.rating += player_b_delta;
		player_a.add_match(*this);
		player_b.add_match(*this);
	}
};
}
