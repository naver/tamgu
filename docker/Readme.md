# Docker

We also provide a `Dockerfile`, together with precompiled versions of Tamgu (see bin) and its libraries for Ubuntu 22.04.

Move into the docker directory. You can then create your own docker image with the command:

You actually only needs this directory to create your docker image. `bin` already contains pre-compiled binaries for Unbutu 22.04.


```bash
docker build -t tamgu-interpreter .
```

To execute the docker:

```bash
docker run -it tamgu-interpreter
```

The script `runwithgui.sh` implements the arguments to launch a specific GUI on top of Tamgu.
