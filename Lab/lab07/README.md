### About Venus
- Run
```
java -jar tools/venus.jar . -dm 
mount local vmfs
```

### You should know that 
- Notice that in neither of the last two exercises did we actually know the cache parameters of our machine. We just made the code exhibit a higher degree of locality, and this magically made things go faster! This tells us that caches, regardless of their specific parameters, will always benefit from operating on code which exhibits a high degree of locality.