# Elo
Elo is a C++11 header-only implementation of the Elo rating system. Since Elo is header-only, it is very easy to add Elo to your project. Elo is licensed under the MIT License. See the `LICENSE` file for more details.

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

When the results come in, record them using the `Match` object, a container for matches. Create a `Match` for each player being rated:
```C++
Elo::Match match;
match.add_result(player2, Elo::DRAW());
match.add_result(player3, Elo::WIN());
Elo::Player new_player1 = system.rate_match(player1, match);
```

`rate_match_k` allows you to specify a *K* value other than the default. For example, if you want *K* = 20 instead, then
```C++
Elo::Match match;
match.add_result(player2, Elo::DRAW());
match.add_result(player3, Elo::WIN());
Elo::Player new_player1 = system.rate_match_k(player1, match, 20);
```

Note that `player2` and `player3` needs to be updated as well, and if `player1` is changed, we cannot rate games against `player1` as they would be calculated against the new rating. Therefore, it is best to keep `new_player1` and all other new ratings separate before updating the ratings.

To get the players' ratings, simply use `get_rating`:
```C++
player1 = new_player1;
double rating = player1.get_rating();
```
## Advanced use
If you just want a rating system, the above instructions will suffice. But Elo comes with the ability to specify an alternative probability distribution for ratings. The default is the logistic distribution with scale = 400 and base = 10 (instead of the constant *e*), but other distributions may be specified. Inherit the `Elo::Distribution` class than override the `cdf` method (the cumulative distribution function). Then pass it to the system. Elo comes with the logistic and normal (Gaussian) distribution.
