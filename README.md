# Computer-Networks

This repository contains source code's corresponding to NITW computer network's curriculum and curation of Computer Network articles over the internet.

Mainly 5 modes of IPC were used
1. FIFO's(named pipes)
2. Message queue's
3. Pipes
4. Semaphores
5. Shared Memory

Introduction to reverse proxy 
https://www.cloudflare.com/learning/cdn/glossary/reverse-proxy/

Introduction to content distribution network
https://www.cloudflare.com/learning/cdn/what-is-a-cdn/

How DNS works ?
https://www.cloudflare.com/learning/dns/what-is-dns/

### DNS(domain name server)
It is the phone book of the internet. converts human readable addresses to machine understandable addresses.
DNS has mainly 4 components
1. recursive DNS resolver
2. root-name-server
3. top-level-domain name server
4. authoritative name server (single source of truth of the ip addresses for the domain)

flow of DNS lookup 
1. the browser cache is looked in to, if not found then,
2. the operating system DNSs cache is looked in to ,
3. now the the DNS request is sent to outside of the local network.
4. once recursive DNS resolver in the ISP receives the query(google.com) then the resolver queries root name server.
5. The rootname server looks for the top-level-domain name server and gives it IP address
6. Then the resolve make a query to the TLD server then it responds with the domain name server address.
7. The resolver makes a query for the domain name and gets the response.
