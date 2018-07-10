#include "elo.hpp"
#include <iostream>
#include <string>
#include <vector>

void show_help() {
	std::cerr << "Usage:" << std::endl
	<< "* elo help - Show this help screen" << std::endl << std::endl
	<< "* elo update [rating A] [rating B] [score of A] <K (default 32)> - Update the ratings of two players in a game. Score is either 1 (A wins), 0.5 (draw), or 0 (B wins). K is an optional parameter that is the maximum change in rating." << std::endl << std::endl
	<< "* elo diff [wins] [draws] [losses] - Estimate the rating difference between a player A and another player B given player A's results against B." << std::endl << std::endl
	<< "* elo diff-interval [wins] [draws] [losses] <percentile (default 0.95)> - Same as elo diff, but it yields a confidence interval (to the specified percentile) of the rating difference in addition to the rating difference estimate." << std::endl;
}

bool check_diff(int wins, int draws, int losses) {
	if (wins < 0 || draws < 0 || losses < 0) {
		std::cerr << "Error: The number of wins, draws, or losses cannot be negative." << std::endl;
		return false;
	}

	if (wins + draws + losses <= 0) {
		std::cerr << "Error: The number of games must be positive." << std::endl;
		return false;
	}

	return true;
}

void print_diff(Elo::IntervalEstimate est, bool with_intervals = false) {
	if (!with_intervals) {
		if (est.estimate > 0) {
			std::cout << "Player A is " << est.estimate << " Elo points stronger than Player B." << std::endl;
		} else if (est.estimate < 0) {
			std::cout << "Player A is " << -est.estimate << " Elo points weaker than Player B." << std::endl;
		} else {
			std::cout << "Player A has the same Elo rating as Player B." << std::endl;
		}
		return;
	}

	if (est.estimate > 0) {
		std::cout << "Player A is " << est.estimate << " (" << est.p * 100 << " percent confidence interval: " << est.lower << ", " << est.upper << ") Elo points stronger than Player B." << std::endl;
	} else if (est.estimate < 0) {
		std::cout << "Player A is " << -est.estimate <<  " (" << est.p * 100 << " percent confidence interval: " << est.lower << ", " << est.upper << ") Elo points weaker than Player B." << std::endl;
	} else {
		std::cout << "Player A has the same Elo rating ("  << est.p * 100 << " percent confidence interval: " << est.lower << ", " << est.upper << ") as Player B." << std::endl;
	}
}

int main(int argc, const char *argv[]) {
	if (argc < 2) {
		show_help();
		return 0;
	}

	std::vector<std::string> args(argv, argv + argc);

	if (args[1] == "help") {
		show_help();
		return 0;

	} else if (args[1] == "update") {
		if (argc < 5 || argc > 6) {
			std::cerr << "elo update [rating A] [rating B] [score of A] <K (default 32)> - Update the ratings of two players in a game. Score is either 1 (A wins), 0.5 (draw), or 0 (B wins). K is an optional positive parameter that is the maximum change in rating." << std::endl;
			return 1;
		}

		double k;
		if (argc == 6) {
			k = std::stod(args[5]);
		} else {
			k = 32;
		}

		if (k <= 0) {
			std::cerr << "Error: K must be positive." << std::endl;
			return 1;
		}

		double rating_a = std::stod(args[2]);
		double rating_b = std::stod(args[3]);
		double score = std::stod(args[4]);

		if (!(score == 0 || score == 0.5 || score == 1)) {
			std::cerr << "Error: score must be either 1 (A wins), 0.5 (draw), or 0 (B wins)." << std::endl;
			return 1;
		}

		Elo::Configuration config(k);
		Elo::Player player_a(rating_a, config);
		Elo::Player player_b(rating_b, config);
		Elo::Match match(player_a, player_b, score);
		match.apply();

		std::cout << "New ratings:" << std::endl << "* Player A: " << player_a.rating << std::endl << "* Player B: " << player_b.rating << std::endl;

	} else if (args[1] == "diff") {
		if (argc != 5) {
			std::cerr << "Usage: elo diff [wins] [draws] [losses] - Estimate the rating difference between a player A and another player B given player A's results against B." << std::endl;
			return 1;
		}

		int wins = std::stoi(args[2]);
		int draws = std::stoi(args[3]);
		int losses = std::stoi(args[4]);

		if (!check_diff(wins, draws, losses)) {
			return 1;
		}

		Elo::IntervalEstimate est = Elo::estimate_rating_difference(wins, draws, losses);
		if (est.estimate_infinity) {
			std::cout << "There have not enough games to yield a rating difference, or the player has only won or lost against the opponent." << std::endl;
			return 0;
		}

		print_diff(est, false);

	} else if (args[1] == "diff-interval") {
		if (argc < 5 || argc > 6) {
			std::cerr << "Usage: elo diff-interval [wins] [draws] [losses] <percentile (default 0.95)> - Same as elo diff, but it yields a confidence interval (to the specified percentile) of the rating difference in addition to the rating difference estimate." << std::endl;
			return 1;
		}

		double p;
		if (argc == 6) {
			p = std::stod(args[5]);
			if (p < 0 || p > 1) {
				std::cerr << "Error: p must be between 0 and 1 (inclusive)." << std::endl;
				return 1;
			}
		} else {
			p = 0.95;
		}

		int wins = std::stoi(args[2]);
		int draws = std::stoi(args[3]);
		int losses = std::stoi(args[4]);

		if (!check_diff(wins, draws, losses)) {
			return 1;
		}

		Elo::IntervalEstimate est = Elo::estimate_rating_difference(wins, draws, losses, p);
		if (est.estimate_infinity) {
			std::cout << "There have not enough games to yield a rating difference, or the player has only won or lost against the opponent." << std::endl;
			return 0;
		}

		print_diff(est, true);

	} else {
		std::cerr << "Error: '" << args[1] << "' is not an elo command. For a list of commands, see 'elo help.'" << std::endl;
		return 1;
	}
	return 0;
}
