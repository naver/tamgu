# Extensiones

Las extensiones te permiten agregar nuevos métodos a ciertos tipos, como map, vector, int, string y otros. El nombre de la extensión debe ser uno de los siguientes tipos válidos: string, automat on, date, time, file, integer, float, vector, list, map, set.

## Definición de la extensión

Para definir una extensión, creas un marco donde declaras los nuevos métodos que deseas agregar al sistema.

Si necesitas hacer referencia al elemento actual, puedes usar una variable cuyo nombre sea el propio tipo con "_" como prefijo. Por ejemplo, si estás extendiendo el tipo "vector", la variable sería "_vector".

Es importante tener en cuenta que si agregas nuevos métodos a "map", todos los mapas heredarán estos nuevos métodos. Lo mismo se aplica a los vectores.

### Ejemplo:

```python
// Extendemos map para devolver un valor, que se eliminará del mapa.
extension map {
    // Agregamos este nuevo método, que estará disponible para todos los mapas...
    function returnAndDelete(int key) {
        // Extraemos el valor con nuestra clave
        string s = _map[key];
        // Lo eliminamos
        _map.pop(key);
        return s;
    }
}

map mx = {1: 2, 3: 4};
// returnAndDelete ahora está disponible para todos los tipos de mapas.
string s = mx.returnAndDelete(1);

imap imx = {1: 2, 3: 4};
int x = imx.returnAndDelete(1);
```

En el ejemplo anterior, extendemos el tipo "map" para agregar un nuevo método llamado "returnAndDelete". Este método devuelve un valor del mapa y lo elimina. Luego, se utiliza el método en un objeto de tipo mapa llamado "mx" y en un objeto de tipo imap llamado "imx".