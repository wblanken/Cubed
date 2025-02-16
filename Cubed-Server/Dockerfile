FROM debian:trixie AS build

RUN apt-get update && apt-get install -y  \
    build-essential  \
    clang \
    lua5.4 \
    lua5.4-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/* 

WORKDIR /cubed

COPY ../Cubed-Server/ ./Cubed-Server/
COPY ../Cubed-Common/ ./Cubed-Common/
COPY ../Walnut/ ./Walnut/
COPY ../Build-Server.lua .

RUN Walnut/vendor/bin/premake/Linux/premake5 --cc=clang --file=Build-Server.lua gmake2

# Set the library path for the linker
ENV LD_LIBRARY_PATH=/cubed/Walnut/Walnut-Modules/Walnut-Networking/vendor/GameNetworkingSockets/bin/Linux
RUN make -j8

FROM debian:trixie-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 libssl3 && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

RUN useradd --system --user-group cubed

COPY --chown=cubed:cubed --from=build ./cubed/bin/Debug-linux-x86_64/Cubed-Server/ ./cubed/
RUN chown -R cubed:cubed /cubed
USER cubed

COPY --chown=cubed:cubed --from=build ./cubed/Walnut/Walnut-Modules/Walnut-Networking/vendor/GameNetworkingSockets/bin/Linux/ /usr/lib/

WORKDIR /cubed

EXPOSE 8192

ENTRYPOINT ["./Cubed-Server"]
