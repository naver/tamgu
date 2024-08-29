# Use Ubuntu 20.04 as the base image
FROM ubuntu:20.04

# Update packages and install system dependencies
RUN apt-get update && apt-get install -y \
    libao-dev \
    libboost-regex-dev \
    libssl-dev \
    libcurl4-openssl-dev \
    libfltk1.3-dev \
    libfontconfig1-dev \
    libgmp-dev \
    libldap2-dev \
    libmpg123-dev \
    libpython3.10-dev \
    libsndfile1-dev \
    libsqlite3-dev \
    libx11-dev \
    libxcursor-dev \
    libxext-dev \
    libxfixes-dev \
    libxft-dev \
    libxinerama-dev \
    libxml2-dev \
    libxrender-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create a directory for the interpreter and its libraries
WORKDIR /app

# Define the TAMGULIBS environment variable
ENV TAMGULIBS=/app

# Copy the interpreter and its libraries from the bin directory to the container
COPY bin/liballmaps.so /app/
COPY bin/liblinear.so /app/
COPY bin/libsqlite.so /app/
COPY bin/libtamgucurl.so /app/
COPY bin/libtamgulispe.so /app/
COPY bin/libtamgupdf.so /app/
COPY bin/libtamgu.so /app/
COPY bin/libwapiti.so /app/
COPY bin/libword2vec.so /app/
COPY bin/libxml.so /app/
COPY bin/pytamgu.so /app/
COPY bin/tamgu /app/

# Define the default command to run the tamgu interpreter
CMD ["/app/tamgu"]
