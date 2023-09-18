# Socket

La classe `Socket` gère les interactions de socket HTML entre un client et un serveur.

## Méthodes

### Côté client

1. `close()`: ferme le socket.
2. `close(clientid)`: ferme la communication avec le `clientid` spécifié.
3. `connect(string hostname, int port)`: connecte un client à un hôte spécifique sur un port spécifique.
4. `createServer(int port, int nbclients)`: crée un serveur sur l'hôte local avec un port spécifique.
5. `createServer(string hostname, int port, int nbclients)`: crée un serveur sur un hôte avec un port spécifique.
6. `get()`: obtient un caractère à partir d'un socket.
7. `get(int clientid)`: obtient un caractère à partir d'un socket avec le `clientid` spécifié.
8. `getFrame(string name)`: renvoie un objet de gestion à distance de la frame spécifiée par `name`.
9. `getFunction(string name)`: renvoie un objet de gestion à distance de la fonction spécifiée par `name`.
10. `getHostname()`: renvoie le nom d'hôte actuel. Le socket n'a pas besoin d'être activé pour obtenir cette information.
11. `read()`: lit un objet Tamgu sur le socket.
12. `read(int clientid)`: lit un objet Tamgu sur le socket avec le `clientid` spécifié.
13. `receive(int nb)`: lit `nb` caractères à partir d'un socket.
14. `receive(int clientid, int nb)`: lit `nb` caractères à partir du socket avec le `clientid` spécifié.
15. `run(int client, string stopstring)`: met le serveur en mode exécution. Le serveur peut maintenant accepter le mode Invocation de Méthode à Distance (RMI).
16. `send(int clientid, string s)`: écrit une chaîne simple sur le socket avec le `clientid` spécifié.
17. `send(string s)`: écrit une chaîne simple sur le socket.

### Côté serveur

18. `setTimeout(int i)`: définit le délai d'attente en secondes pour l'écriture et la lecture sur le socket. Utilisez cette instruction pour éviter le blocage lors d'une lecture ou d'une écriture. Une valeur de -1 annule le délai d'attente.
19. `wait()`: le serveur attend qu'un client se connecte. Il renvoie l'identifiant du client, qui sera utilisé pour communiquer avec le client.
20. `write(clientid, o1, o2...)`: écrit des objets Tamgu sur le socket avec le `clientid` spécifié.
21. `write(o1, o2...)`: écrit des objets Tamgu sur le socket.

## Exemple : Côté serveur

```python
// Côté serveur
int clientid;
Socket s; // crée un socket
string name = s.getHostname();
println("Serveur local :", name);
// Crée un serveur sur le socket 2020, avec au maximum 5 connexions
s.createServer(2020, 5);
// Attend une connexion client
while (true) {
  // Accepte jusqu'à 5 connexions
  clientid = s.wait();
  // Lit un message du client (devrait être fait dans un thread pour gérer plus de connexions)
  string message = s.read(clientid);
  message += " et renvoyé";
  // Écrit un message vers le client
  s.write(clientid);
  // Ferme la connexion
  s.close(clientid);
}
// Ferme le serveur
s.close();
```

## Exemple : Côté client

```python
// Côté client
Socket s; // crée un socket
string name = s.getHostname();
println("Serveur local :", name);
// Crée un serveur sur le socket 2020
s.connect(name, 2020);
// Écrit un message vers le serveur
string message = "Bonjour";
s.write(message);
// Lit un message du serveur
message = s.read();
println(message);
// Ferme la connexion
s.close();
```