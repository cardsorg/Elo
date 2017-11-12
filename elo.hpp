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
#include <exception>
#include <iostream>
#include <vector>

namespace Elo {

double WIN(int times = 1) {
	return 1.0 * times;
}

double DRAW(int times = 1) {
	return 0.5 * times;
}

double LOSS() {
	return 0;
}
double round_places(double x, double places) {
	if (x == 0) {
		return 0;
	}

	double scale = std::pow(10.0, places);
	return std::round(x * scale) / scale;
}

class Player {
	double rating;

public:
	Player(double initial_rating):
		rating(initial_rating) {};

	double get_rating() {
		return rating;
	}

	void set_rating(double new_rating) {
		rating = new_rating;
	}

	double round_rating(double places) {
		return round_places(rating, places);
	}
};

class Distribution {
public:
	virtual double cdf(double x, double mean) { return 0; };
	virtual ~Distribution() {};
};

class LogisticDistribution : public Distribution {
	double base;
	double scale;

public:
	LogisticDistribution(double initial_base, double initial_scale):
		base(initial_base), scale(initial_scale) {};

	double get_base() {
		return base;
	}

	double get_scale() {
		return scale;
	}

	void set_base(double new_base) {
		base = new_base;
	}

	void set_scale(double new_scale) {
		scale = new_scale;
	}

	virtual double cdf(double x, double mean) override {
		return 1.0 / (1.0 + std::pow(base, -((x - mean) / scale)));
	}
};

class NormalDistribution : public Distribution {
	double stdev;

public:
	NormalDistribution(double initial_stdev):
		stdev(initial_stdev) {};

	double get_stdev() {
		return stdev;
	}

	void set_stdev(double new_stdev) {
		stdev = new_stdev;
	}

	virtual double cdf(double x, double mean) override {
		return (1 + std::erf((x - mean) / (stdev * std::sqrt(2)))) / 2;
	}
};

LogisticDistribution default_distribution(10, 400);

class System {
	Distribution* dist;
	double default_k;

public:
	System(double initial_default_k = 32, Distribution *initial_dist = &default_distribution):
		dist(initial_dist), default_k(initial_default_k) {};

	Distribution get_distribution() {
		return *dist;
	}

	void set_distribution(Distribution new_dist) {
		dist = &new_dist;
	}

	void set_distribution(Distribution* new_dist) {
		dist = new_dist;
	}

	double get_default_k() {
		return default_k;
	};

	void set_default_k(double new_default_k) {
		default_k = new_default_k;
	}

	double expected_score(Player a, Player b) {
		return 1 - dist->cdf(b.get_rating(), a.get_rating());
	}

	Player rate_k(Player player, std::vector<Player> opponents, std::vector<double> scores, double k) {
		double expected_sum = 0.0;
		for (int i = 0; i < opponents.size(); i++) {
			expected_sum += expected_score(player.get_rating(), opponents[i].get_rating());
			std::cout << expected_sum << std::endl;
		}

		double score_sum = 0.0;
		for (int i = 0; i < scores.size(); i++) {
			score_sum += scores[i];
		}

		return Player (player.get_rating() + k * (score_sum - expected_sum));
	}

	Player rate(Player player, std::vector<Player> opponents, std::vector<double> scores) {
		return rate_k(player, opponents, scores, default_k);
	}

	void update_k(Player& player, std::vector<Player> opponents, std::vector<double> scores, double k) {
		player = rate_k(player, opponents, scores, k);
	}

	void update(Player& player, std::vector<Player> opponents, std::vector<double> scores) {
		player = rate(player, opponents, scores);
	}
};
}
