# Elo
Elo is a C++11 header-only implementation of the Elo rating system. Since Elo is header-only, it is very easy to add Elo to your project. Elo is licensed under the MIT License. See the `LICENSE` file for more details.

Create a `Configuration` object to configure the *K* constant of the Elo system. *K* specifies the maximum change in rating per game. For example, if *K* = 32, then each player can only gain or lose at most 32 rating points for each game they play.

```C++
Elo::Configuration config1(32);
Elo::Configuration config2(16);
Elo::Configuration config3(64);
```

For each player being rated, initialize a `Player` object. The `Player` object constructor takes the player's initial rating (first argument) and the configuration that will be used (second). In most cases, you will want to create a single configuration and apply it to each player.

```C++
Elo::Player player1 (1500, config1);
Elo::Player player2 (1600, config2);
Elo::Player player3 (1300, config3);
```

For each game that is played, construct a `Match` object. The match object takes two `Player` objects and the score (1 for a win, 0.5 for a draw, 0 for a loss) of the **first player**. For convenience, the three constants `WIN`, `DRAW`, and `LOSS` are given.

```C++
Elo::Match match1(player1, player2, Elo::WIN);
```

To update the ratings of each player, you can either pass `true` as the fourth argument to the `Match` constructor, which automatically updates the players' ratings, or call `Match::apply()`:

```C++
match1.apply();
```

If the ratings have already been updated, `Match::apply()` returns `false`.

To get a player's rating, simply access the `rating` variable:

```C++
std::cout << player1.rating << std::endl;
```

## Advanced use
### Custom distributions
If you just want a rating system, the above instructions will suffice. But Elo comes with the ability to specify an alternative probability distribution for ratings. The default is the logistic distribution with scale = 400 and base = 10 (instead of the constant *e*). Most users use this distribution, like the US Chess Federation.

Other distributions may be specified. For example, the Féderation Internationale des Échecs (FIDE) uses a piecewise normal distribution and this will have to be specified if you want to rate FIDE (international chess) games. Inherit the `Elo::Distribution` class than override the `cdf` method (the cumulative distribution function). Then pass it to the `Configuration` as the second argument. Elo comes with the logistic and normal (Gaussian) distribution.

### Estimating rating difference
Let us suppose that two players have played a series of *n* games against each other, and you wish to know the approximate Elo rating difference between the two players. Elo comes with a function `Elo::estimate_rating_difference()` that lets you do just that.

```C++
Elo::IntervalEstimate diff = Elo::estimate_rating_difference(10, 5, 8, p=0.95)
```

In the above, a player has won 10 games, drawn 5 games, and lost 8 games against one other player. We want to know how many Elo points stronger or weaker this player is than another player, and we want to get a 95 percent confidence interval on this difference. This function does just that. The estimated difference is stored in `diff.estimate`, the lower bound of the interval is `diff.lower`, and the upper bound of the interval is `diff.upper`.

In some cases, the player might have won or lost all their games against another player. In this case, no estimate can be made because the system needs to see both at least a win and a loss to calculate an estimate (a draw is considered as 1 win and 1 loss in the Elo system). When this happens, `diff.estimate_infinity` is `true` to indicate that an estimate cannot be made.

### Command line interface
Elo comes with a rudimentary command-line interface. Simply run the `Makefile` and run the `elo` program for documentation.

### Mathematical details
`Elo::estimate_rating_difference()` uses the Wilson confidence interval for a Bernoulli parameter.
