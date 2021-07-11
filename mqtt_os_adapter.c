#include "os_config.h"
#include "mqtt_os_adapter.h"



#ifdef OS_LINUX

#ifdef SOCKET_LINUX

char expired(Timer* timer)
{
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&timer->end_time, &now, &res);		
	return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
}


void countdown_ms(Timer* timer, unsigned int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timeval interval = {timeout / 1000, (timeout % 1000) * 1000};
	timeradd(&now, &interval, &timer->end_time);
}


void countdown(Timer* timer, unsigned int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timeval interval = {timeout, 0};
	timeradd(&now, &interval, &timer->end_time);
}


int left_ms(Timer* timer)
{
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&timer->end_time, &now, &res);
	//printf("left %d ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
	return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}


void InitTimer(Timer* timer)
{
	timer->end_time = (struct timeval){0, 0};
}


int linux_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
	//mDEBUG("[%s]timeout_ms = %d \n", __FUNCTION__, timeout_ms);
	if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 0))
	{
		interval.tv_sec = 0;
		interval.tv_usec = 100;
	}

	setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval));

	int bytes = 0;
	while (bytes < len)
	{

		int rc = recv(n->my_socket, &buffer[bytes], (size_t)(len - bytes), 0);
		//mDEBUG("[%s]socket recv rc = %d \n", __FUNCTION__, rc);

		if(rc == 0)
		{
			 //mDEBUG("[%s]socket close \n", __FUNCTION__);
			 bytes = 0;
			 break;
		}
		
		if (rc == -1)
		{
			if (errno != ENOTCONN && errno != ECONNRESET)
			{
				bytes = -1;
				break;
			}
		}
		else
			bytes += rc;
	}
	return bytes;
}


int linux_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	struct timeval tv;

	tv.tv_sec = 0;  /* 30 Secs Timeout */
	tv.tv_usec = timeout_ms * 1000;  // Not init'ing this can cause strange errors

	setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

	int	rc = write(n->my_socket, buffer, len);

	return rc;
}


void linux_disconnect(Network* n)
{
	close(n->my_socket);
}


void NewNetwork(Network* n)
{
	n->my_socket = 0;
	n->mqttread = linux_read;
	n->mqttwrite = linux_write;
	n->disconnect = linux_disconnect;
}


int ConnectNetwork(Network* n, char* addr, int port)
{
	int type = SOCK_STREAM;
	struct sockaddr_in address;
	int rc = -1;
	sa_family_t family = AF_INET;
	struct addrinfo *result = NULL;
	struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

	if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0)
	{
		struct addrinfo* res = result;
		/* prefer ip4 addresses */
		while (res)
		{
			if (res->ai_family == AF_INET)
			{
				result = res;
				break;
			}
			res = res->ai_next;
		}
	
		if (result->ai_family == AF_INET)
		{
			address.sin_port = htons(port);
			address.sin_family = family = AF_INET;
			address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
		}
		else
			rc = -1;

		freeaddrinfo(result);
	}

	if (rc == 0)
	{
		n->my_socket = socket(family, type, 0);
		if (n->my_socket != -1)
		{
			int opt = 1;			
			rc = connect(n->my_socket, (struct sockaddr*)&address, sizeof(address));
		}
	}

	return rc;
}


#endif

#ifdef SOCKET_AT

char expired(Timer* timer)
{
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&timer->end_time, &now, &res);		
	return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
}


void countdown_ms(Timer* timer, unsigned int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timeval interval = {timeout / 1000, (timeout % 1000) * 1000};
	timeradd(&now, &interval, &timer->end_time);
}


void countdown(Timer* timer, unsigned int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timeval interval = {timeout, 0};
	timeradd(&now, &interval, &timer->end_time);
}


int left_ms(Timer* timer)
{
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&timer->end_time, &now, &res);
	//printf("left %d ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
	return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}


void InitTimer(Timer* timer)
{
	timer->end_time = (struct timeval){0, 0};
}


int linux_at_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
	//mDEBUG("[%s]timeout_ms = %d \n", __FUNCTION__, timeout_ms);
	if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 0))
	{
		interval.tv_sec = 0;
		interval.tv_usec = 100;
	}

	return 1;
}


int linux_at_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	struct timeval tv;

	tv.tv_sec = 0;  /* 30 Secs Timeout */
	tv.tv_usec = timeout_ms * 1000;  // Not init'ing this can cause strange errors



	return rc;
}


void linux_at_disconnect(Network* n)
{
	close(n->my_socket);
}


void NewNetwork(Network* n)
{
	n->my_socket = 0;
	n->mqttread = linux_at_read;
	n->mqttwrite = linux_at_write;
	n->disconnect = linux_at_disconnect;
}


int ConnectNetwork(Network* n, char* addr, int port)
{
	int rc = -1;

	return rc;
}


#endif

#endif

#ifdef OS_RTOS


char expired(Timer* timer)
{

}


void countdown_ms(Timer* timer, unsigned int timeout)
{

}


void countdown(Timer* timer, unsigned int timeout)
{

}


int left_ms(Timer* timer)
{

}


void InitTimer(Timer* timer)
{

}


int rtos_at_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{

	return -1;
}


int rtos_at_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{

	return -1;
}


void rtos_at_disconnect(Network* n)
{

}


void NewNetwork(Network* n)
{
	n->my_socket = 0;
	n->mqttread = rtos_at_read;
	n->mqttwrite = rtos_at_write;
	n->disconnect = rtos_at_disconnect;
}


int ConnectNetwork(Network* n, char* addr, int port)
{
	int rc = -1;

	return rc;
}



#endif
