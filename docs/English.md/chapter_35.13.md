## GPSdistance

The `GPSdistance` function calculates the distance between two GPS points provided as longitude and latitude coordinates. The function signature is as follows:

```cpp
float GPSdistance(float Longitude1, float Latitude1, float Longitude2, float Latitude2, float radius)
```

The first four parameters, `Longitude1`, `Latitude1`, `Longitude2`, and `Latitude2`, represent the longitude and latitude coordinates of the two GPS points. The fifth parameter, `radius`, is optional and represents the Earth radius. The default value for `radius` is 6371 km. This value also determines the unit used to compute the final distance value.

### Example

```cpp
float distance = GPSdistance(46.9001, 7.1201, 47.01, 6.1);
```

In this example, the distance between the GPS points (46.9001, 7.1201) and (47.01, 6.1) is calculated. The resulting distance is 78.382 km.