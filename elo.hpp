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
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <vector>

namespace Elo {

double WIN(std::size_t times = 1) {
	return 1.0 * times;
}

double DRAW(std::size_t times = 1) {
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

class Match {
public:
	std::vector<Player> opponents;
	std::vector<double> scores;

	Match() {};

	Match(std::vector<Player> initial_opponents, std::vector<double> initial_scores):
		opponents(initial_opponents), scores(initial_scores) {};

	Match(std::initializer_list<Player> initial_opponents, std::initializer_list<double> initial_scores):
		opponents(initial_opponents), scores(initial_scores) {};

	Match(Player initial_opponent, double initial_score) {
		opponents.push_back(initial_opponent);
		scores.push_back(initial_score);
	}

	std::vector<Player> get_opponents() {
		return opponents;
	}

	std::vector<double> get_scores() {
		return scores;
	}

	void add_opponent(Player opponent) {
		opponents.push_back(opponent);
	}

	void add_score(double score) {
		scores.push_back(score);
	}

	void add_opponent_score(Player opponent, double score) {
		add_opponent(opponent);
		add_score(score);
	}

	template <typename Iterable>
	void add_opponents(Iterable new_opponents) {
		opponents.insert(opponents.end(), std::begin(new_opponents), std::end(new_opponents));
	}

	template <typename Iterable>
	void add_scores(Iterable new_scores) {
		scores.insert(scores.end(), std::begin(new_scores), std::end(new_scores));
	}

	template <typename Iterable1, typename Iterable2>
	void add_opponents_scores(Iterable1 new_opponents, Iterable2 new_scores) {
		add_opponents(new_opponents);
		add_scores(new_scores);
	}
};

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

	template <typename Iterable1, typename Iterable2>
	Player rate_list_k(Player match_player, Iterable1 opponents, Iterable2 scores, double k) {
		if (opponents.size() != scores.size()) {
			throw std::invalid_argument("Number of opponents does not equal number of scores.");
		}

		double expected_sum = 0.0;
		for (auto opponent : opponents) {
			expected_sum += expected_score(match_player.get_rating(), opponent.get_rating());
			std::cout << expected_sum << std::endl;
		}

		double score_sum = 0.0;
		for (auto score : scores) {
			score_sum += score;
		}

		return Player (match_player.get_rating() + k * (score_sum - expected_sum));
	}

	template <typename Iterable1, typename Iterable2>
	Player rate_list(Player match_player, Iterable1 opponents, Iterable2 scores) {
		return rate_list_k(match_player, opponents, scores, default_k);
	}

	Player rate_match_k(Player match_player, Match match, double k) {
		return rate_list_k(match_player, match.get_opponents(), match.get_scores(), k);
	}

	Player rate_match(Player match_player, Match match) {
		return rate_list(match_player, match.get_opponents(), match.get_scores());
	}
};
}
