How DNS works ?
https://www.cloudflare.com/learning/dns/what-is-dns/

### DNS(domain name server)
It is the phone book of the internet. converts human readable addresses to machine understandable addresses.
DNS has mainly 4 components
1. recursive DNS resolver
2. root-name-server
3. top-level-domain name server
4. authoritative name server (single source of truth of the ip addresses for the domain)

Flow of DNS lookup 
1. the browser cache is looked in to, if not found then,
2. the operating system DNS cache is looked in to ,
3. now the the DNS request is sent to outside of the local network.
4. once recursive DNS resolver in the ISP receives the query(google.com) then the resolver queries root name server.
5. The rootname server looks for the top-level-domain name server and gives it IP address(.com)
6. Then the resolve make a query to the TLD server then it responds with the domain name server address(google.com).
7. the final ip address is found at authoritative domain name server.
8. The resolver makes a http request to the domain's ip address and gets the response.
