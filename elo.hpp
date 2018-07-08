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
#include <initializer_list>
#include <list>
#include <iterator>
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

struct Configuration {
	Distribution& dist;
	double k;

	Configuration(Distribution& initial_distribution, double initial_k):
		dist(initial_distribution), k(initial_k) {};
};

Configuration default_configuration(default_distribution, 32);

class Player;
struct Match {
	Player& opponent;
	// This is the result for player_a.
	double result;

	Match(Player &initial_opponent, double initial_result):
		opponent(initial_opponent), result(initial_result) {};
};

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

	void add_match(Match m) {
		matches.push_back(m);
		// Update rating.
		rating += config.k * (m.result - config.dist.cdf(m.opponent.rating, rating));
	}

	std::vector<Match> get_matches() {
		return matches;
	}
};
}
