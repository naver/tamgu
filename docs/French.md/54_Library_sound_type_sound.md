# Bibliothèque Sound

La bibliothèque Sound dans Tamgu permet de lire différents types de fichiers son tels que WAV, MP3, FLAC, OGG, etc. Vous pouvez facilement charger un fichier et le lire n'importe où dans votre code en utilisant le type "sound".

**Note :** Tamgu dépend des bibliothèques suivantes pour le décodage et la lecture : libao4, libsndfile-1 et libmpg123.

Pour utiliser cette bibliothèque, vous pouvez l'appeler en utilisant `use("sound")`.

## Méthodes

L'API expose les méthodes suivantes :

1. `close()`: Ferme un canal sonore.
2. `decode(ivector soundbuffer)`: Décode le fichier sonore et retourne le contenu buffer par buffer dans `soundbuffer`. Renvoie false lorsque la fin du fichier est atteinte.
3. `encode(ivector soundbuffer)`: Joue un buffer sonore renvoyé par `decode`.
4. `load(string pathname)`: Charge le son à partir du chemin d'accès spécifié `pathname`.
5. `open(map params)`: Ouvre un canal sonore avec les paramètres du fichier sonore actuel (voir les paramètres).
6. `parameters()`: Renvoie les paramètres du fichier sonore actuel sous forme de map.
7. `parameters(map modifs)`: Seules les clés "rate" et "channels" peuvent être modifiées.
8. `play()`: Joue le son.
9. `play(bool beg)`: Joue le son depuis le début.
10. `play(ivector soundbuffer)`: Joue le buffer sonore (voir `encode`).
11. `reset()`: Réinitialise le fichier sonore au début.
12. `stop()`: Arrête le son. Il est nécessaire de lire le fichier sonore dans un thread pour utiliser cette instruction.

## Exemple

```cpp
use("sound");
sound s;
s.load("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");
s.play();
```

Vous pouvez également charger un son avec la déclaration suivante :

```cpp
sound s("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");
```

## Exemple de décodage

```cpp
use("sound");

// Ouvrir un fichier sonore
sound s("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");

// Ouvrir un deuxième canal sonore
sound c;

// Obtenir les paramètres sonores
map params = s.parameters();

// Utiliser les paramètres sonores pour ouvrir un canal
c.open(params);

// Boucle avec le décodage dans le fichier sonore
// et pour chaque nouveau buffer, jouer le son
// Nous pourrions utiliser "play" à la place de "encode",
// mais c'est un peu plus lent
ivector snd;
while (s.decode(snd))
    c.encode(snd);

// Fermer le canal
c.close();
```