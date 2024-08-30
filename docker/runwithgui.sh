# You may need first to  create a .Xauthority directory on your machine
# either with
# xhost +local:root
# or
# xauth generate :0 . trusted

# Then, move to the directory where your file is:

# For instance, if we want to execute birds.tmg (in examples/miscelleneous)
# Some explanations:
# -v $(pwd):/app/host mounts your local directory into the container as /app/host
# -e DISPLAY=$DISPLAY  sets the DISPLAY to our local display
#  -v /tmp/.X11-unix:/tmp/.X11-unix Mount the X11 socket from the host into the container.
#  -v ~/.Xauthority:/root/.Xauthority Mount the X11 authentication file from the host into the container. 
# -console: launch tamgu in a GUI

docker run -it -v $(pwd):/app/host -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -v ~/.Xauthority:/root/.Xauthority tamgu-interpreter -console


