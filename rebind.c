#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>



// expected value "REBIND_PORT=FROM:TO"
#define REBIND_PORT_ENV "REBIND_PORT"

static int(*_bind)(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = NULL;

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	if (NULL == _bind) {
		_bind = (int (*)(int sockfd, const struct sockaddr *addr, socklen_t addrlen)) dlsym(RTLD_NEXT, "bind");
	}
        struct sockaddr_in rebindaddr;

        // some different socket type or other incompatibility
        if (addr->sa_family != AF_INET || addrlen != sizeof(rebindaddr)) goto fallback;

	char *rebind_port_env = getenv(REBIND_PORT_ENV);
        char *sep = NULL;
        char *to_port_c = NULL;
      
	if (NULL == rebind_port_env) goto fallback; //no env var
	if (NULL == (sep = strchr(rebind_port_env,':'))) goto fallback; // no ':' separator
        *sep = 0;
        to_port_c = sep + 1;

	in_port_t from_port = (in_port_t)strtol(rebind_port_env, &sep, 10);
        if (*sep != '\0') goto fallback; //there is symbol ':', but string before it is not a number
	in_port_t to_port = (in_port_t)strtol(to_port_c, &sep, 10);
        if (*sep != '\0') goto fallback; //there is symbol ':', but string after it is not a number


        if (((struct sockaddr_in*)addr)->sin_port != htons(from_port)) goto fallback; //request port doesnt match REBIND_PORT_FROM

	memcpy(&rebindaddr, addr, addrlen);
        rebindaddr.sin_port=htons(to_port);
        fprintf(stderr, "Rebound port from %d to %d\n", from_port, to_port);
        fflush(stderr);
        return _bind(sockfd, (struct sockaddr*) &rebindaddr, sizeof(rebindaddr));
fallback: 
	return _bind(sockfd, addr, addrlen);
}
