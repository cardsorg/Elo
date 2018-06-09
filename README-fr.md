# Elo
Elo est un logiciel pour la méthode de classement d'Elo. Parce que Elo a seulement le fichier d'en-tête, c'est facile d'ajouter Elo à votre projet. Vous pouvez utiliser Elo si vous suivez le permis de MIT. Voir le fichier `LICENSE` pour les détails.

Créer un objet de Player pour chaque joueur:

```C++
Elo::Player player1 (1500);
Elo::Player player2 (1600);
Elo::Player player3 (1300);
```

Créer un objet de System qui enregistre les paramètres. La valeur *K* spécifie la variation maximal dans le classement par l'événement (un série des matches contre les adversaires différents). Par exemple, pour *K* = 40:

```C++
Elo::System system (40);
```

Enregistrer les résultats avec l'objet `Match`. Créer un `Match` pour chaque match.
```C++
Elo::Match match;
match.add_result(player2, Elo::DRAW());
match.add_result(player3, Elo::WIN());
Elo::Player new_player1 = system.rate_match(player1, match);
```

Vous pouvez utiliser `rate_match_k` de spécifier une valeur de *K* qui n'est la défaut. Par exemple, si vous pouvez que *K* = 20, vous pouvez donc écrire:
```C++
Elo::Match match;
match.add_result(player2, Elo::DRAW());
match.add_result(player3, Elo::WIN());
Elo::Player new_player1 = system.rate_match_k(player1, match, 20);
```

Vous devez utiliser les classements vieux avant actualiser les classements. Créer les objets nouveaux des `Player`, calculer les classements nouveaux, et actualiser les classements vieux.

Utiliser `get_rating` d'extraire les classements d'un joueur.
```C++
player1 = new_player1;
double rating = player1.get_rating();
```
