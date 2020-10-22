# udpmask

Minimal UDP packet obfuscation tool for bypassing deep packet inspection.
No external dependencies (including pthread), can be compiled to run on
embedded devices.

## Usage

Server side

    udpmask -m server -c host-to-transfer-traffic-to -o port-to-transfer-traffic-to -p 51194

Client side

    udpmask -m client -c udpmask-server-host -o 51194 -p 61194

On client side, configure the software you wants to obfuscate traffic for to
connect to localhost:61194.

## Use case

* Obfuscate OpenVPN UDP traffic
* Obfuscate WireGuard traffic
