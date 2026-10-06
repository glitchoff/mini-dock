FROM gcc:latest AS build
WORKDIR /src
COPY src/ src/
RUN g++ -std=c++17 -o minidock src/main.cpp

FROM debian:stable-slim
COPY --from=build /src/minidock /usr/local/bin/minidock
CMD ["minidock"]