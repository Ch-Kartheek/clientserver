# Use a base image with necessary tools and libraries
FROM fedora:39

# Install necessary packages
RUN dnf install -y \
    gcc-c++ \
    sqlite \
    sqlite-devel \
    sqlite-libs \
    && dnf clean all

# Copy the source code into the container
COPY . /app/

# Set the working directory
WORKDIR /app/

# Compile the server and client
RUN g++ server.cpp -o server -lsqlite3 && \
    g++ client.cpp -o client -lsqlite3

# Expose ports for server
EXPOSE 6080

# Command to run the server by default
CMD ["./server"]
