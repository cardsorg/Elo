# Elo
Elo is a C++ header-only implementation of the Elo rating system. Since Elo is header-only, it is very easy to add Elo to your project. Elo is licensed under the MIT License. See the `LICENSE` file for more details.

For each player being rated, initialize a Player object:

```C++
Elo::Player player1 (1500);
Elo::Player player2 (1600);
Elo::Player player3 (1300);

```

Now create a System object which stores the parameters of the Elo system. The *K* value specifies the maximum change in rating per event (i.e., a series of games against different opponents). For example, to set *K* = 40:

```C++
Elo::System system (40);
```

When the results come in, update each player's rating using the following methods of `Elo::System`:
```C++
Player rate_k (Player player, std::vector<Player> opponents, std::vector<double> scores, double k);
Player rate(Player player, std::vector<Player> opponents, std::vector<double> scores);
void update_k(Player& player, std::vector<Player> opponents, std::vector<double> scores, double k);
void update(Player& player, std::vector<Player> opponents, std::vector<double> scores)
```

`rate` returns a new `Elo::Player` object with the new rating, while `update` modifies the player object to the new rating. `rate_k` and `update_k` allow you to specify a `K` value other than the default. The `player` parameter is the player being rated, `opponents` is the list of opponents, and `scores` are the list of scores against each opponent. Use `Elo::WIN()` to record a win, `Elo::DRAW()` to record a draw, and `ELO::LOSS()` to record a loss.

For example, using the System above, if `player1` above draws against `player2` and wins against `player3`, we update their rating in one of the following ways.

```C++
player1 = system.rate(player1, {player2, player3}, {Elo::DRAW(), Elo::WIN()});
system.update(player1, {player2, player3}, {Elo::DRAW(), Elo::WIN()});
Elo::Player new_player1 = system.rate(player1, {player2, player3}, {Elo::DRAW(), Elo::WIN()});
```

Although all three lines have the same effect, note that `player2` and `player3` needs to be updated as well, and if `player1` is changed, we cannot rate games against `player1` as they would be calculated against the new rating. Therefore, it is best to create a temporary player, which the third line does.

## Advanced use
If you just want a rating system, the above instructions will suffice. But Elo comes with the ability to specify an alternative probability distribution for ratings. The default is the logistic distribution with scale = 400 and base = 10 (instead of the constant *e*), but other distributions may be specified. Inherit the `Elo::Distribution` class than override the `cdf` method (the cumulative distribution function). Then pass it to the system. Elo comes with the logistic and normal (Gaussian) distribution.
