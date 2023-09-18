## Type de Date

Le type `Date` est utilisé pour manipuler les dates.

### Méthodes

1. `date()`: Renvoie la date sous forme de chaîne de caractères.
2. `day()`: Renvoie le jour sous forme d'entier.
3. `format(string f)`: Renvoie la date formatée sous forme de chaîne de caractères. La chaîne de format utilise une combinaison d'options. Voir ci-dessous pour une explication.
4. `hour()`: Renvoie l'heure sous forme d'entier.
5. `min()`: Renvoie les minutes sous forme d'entier.
6. `month()`: Renvoie le mois sous forme d'entier.
7. `sec()`: Renvoie les secondes sous forme d'entier.
8. `setdate(year, month, day, hour, min, sec)`: Définit les variables de date et d'heure.
9. `year()`: Renvoie l'année sous forme d'entier.
10. `yearday()`: Renvoie le jour de l'année sous forme d'entier entre 0 et 365.
11. `weekday()`: Renvoie le jour de la semaine sous forme d'entier entre 0 et 6, où 0 représente dimanche.

### Opérateurs

- `+` et `-`: Les dates peuvent être ajoutées ou soustraites.

### Options de format

- `%a`: Le nom abrégé du jour de la semaine selon la locale actuelle.
- `%A`: Le nom complet du jour de la semaine selon la locale actuelle.
- `%b`: Le nom abrégé du mois selon la locale actuelle.
- `%B`: Le nom complet du mois selon la locale actuelle.
- `%c`: La représentation préférée de la date et de l'heure pour la locale actuelle.
- `%C`: Le numéro du siècle (année/100) sous forme d'entier à 2 chiffres.
- `%d`: Le jour du mois sous forme de nombre décimal (de 01 à 31).
- `%D`: Équivalent à `%m/%d/%y`.
- `%e`: Comme `%d`, le jour du mois sous forme de nombre décimal, mais un zéro initial est remplacé par un espace.
- `%F`: Équivalent à `%Y-%m-%d` (le format de date ISO 8601).
- `%G`: L'année basée sur la semaine ISO 8601 avec le siècle sous forme d'entier décimal.
- `%g`: Comme `%G`, mais sans le siècle, c'est-à-dire avec une année à 2 chiffres (00-99).
- `%h`: Équivalent à `%b`.
- `%H`: L'heure sous forme de nombre décimal utilisant une horloge de 24 heures (de 00 à 23).
- `%I`: L'heure sous forme de nombre décimal utilisant une horloge de 12 heures (de 01 à 12).
- `%j`: Le jour de l'année sous forme de nombre décimal (de 001 à 366).
- `%k`: L'heure (horloge de 24 heures) sous forme de nombre décimal (de 0 à 23) ; les chiffres simples sont précédés d'un espace.
- `%l`: L'heure (horloge de 12 heures) sous forme de nombre décimal (de 1 à 12) ; les chiffres simples sont précédés d'un espace.
- `%m`: Le mois sous forme de nombre décimal (de 01 à 12).
- `%M`: Les minutes sous forme de nombre décimal (de 00 à 59).
- `%n`: Un caractère de saut de ligne.
- `%p`: Soit "AM" soit "PM" selon la valeur de temps donnée.
- `%P`: Comme `%p` mais en minuscules.
- `%r`: L'heure en notation a.m. ou p.m.
- `%R`: L'heure en notation de 24 heures.
- `%s`: Le nombre de secondes depuis l'Époque, 1970-01-01 00:00:00 +0000 (UTC).
- `%S`: Les secondes sous forme de nombre décimal (de 00 à 60).
- `%t`: Un caractère de tabulation.
- `%T`: L'heure en notation de 24 heures.
- `%u`: Le jour de la semaine sous forme de nombre décimal, de 1 à 7, le lundi étant 1.
- `%U`: Le numéro de la semaine de l'année en cours sous forme de nombre décimal, de 00 à 53, en commençant par le premier dimanche comme premier jour de la semaine 01.
- `%V`: Le numéro de semaine de l'année en cours selon la norme ISO 8601 sous forme de nombre décimal, de 01 à 53, où la semaine 1 est la première semaine qui compte au moins 4 jours dans la nouvelle année.
- `%w`: Le jour de la semaine sous forme de nombre décimal, de 0 à 6, le dimanche étant 0.
- `%W`: Le numéro de la semaine de l'année en cours sous forme de nombre décimal, de 00 à 53, en commençant par le premier lundi comme premier jour de la semaine 01.
- `%x`: La représentation préférée de la date pour la locale actuelle sans l'heure.
- `%X`: La représentation préférée de l'heure pour la locale actuelle sans la date.
- `%y`: L'année sous forme de nombre décimal sans le siècle (de 00 à 99).
- `%Y`: L'année sous forme de nombre décimal incluant le siècle.
- `%z`: Le fuseau horaire numérique (décalage horaire en heures et minutes par rapport à l'UTC).
- `%Z`: Le fuseau horaire ou le nom ou l'abréviation.
- `%+`: La date et l'heure au format de date.
- `%%`: Un caractère '%' littéral.

### Exemples

Exemple 1:

```python
Date d;
println(d.format("%Y%m%d")); // Affiche la date pour le 25/12/2015 comme 20151225
```

Exemple 2:

```python
Date mytime;
print(mytime); // Affiche: 2010/07/08 15:19:22
```