/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2015
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "stm32f4_ethernet.h"

/* Private variables */
static __IO uint32_t EthernetLocalTime = 0;
static __IO uint8_t Initialized = 0;
static __IO uint8_t DNS_Working = 0;
ETHERNET_t ETHERNET;

/* Private functions */
void ETHERNET_INT_DNSFoundCallback(const char *name, struct ip_addr *ipaddr, void *callback_arg);

ETHERNET_Result_t ETHERNET_Init(uint8_t* mac_addr, uint8_t* ip_addr, uint8_t* gateway, uint8_t* netmask) {
	uint8_t i;
	ETHERNET_Result_t result = ETHERNET_Result_Ok;
	
	/* Initialize delay */
	DELAY_Init();

	/* Fill settings */
	/* MAC address */
	if (mac_addr != NULL) {
		/* User selected MAC */
		for (i = 0; i < 6; i++) {
			ETHERNET.mac_addr[i] = mac_addr[i];
		}
	} else {
		/* Default MAC */
		ETHERNET.mac_addr[0] = MAC_ADDR0;
		ETHERNET.mac_addr[1] = MAC_ADDR1;
		ETHERNET.mac_addr[2] = MAC_ADDR2;
		ETHERNET.mac_addr[3] = MAC_ADDR3;
		ETHERNET.mac_addr[4] = MAC_ADDR4;
		ETHERNET.mac_addr[5] = MAC_ADDR5;
	}
	
	/* IP */
	if (ip_addr != NULL) {
		/* User selected IP */
		for (i = 0; i < 4; i++) {
			ETHERNET.ip_addr[i] = ip_addr[i];
		}
	} else {
		/* Default IP */
		ETHERNET.ip_addr[0] = IP_ADDR0;
		ETHERNET.ip_addr[1] = IP_ADDR1;
		ETHERNET.ip_addr[2] = IP_ADDR2;
		ETHERNET.ip_addr[3] = IP_ADDR3;
	}
		
	/* Gateway */
	if (gateway != NULL) {
		/* User selected Gateway */
		for (i = 0; i < 4; i++) {
			ETHERNET.gateway[i] = gateway[i];
		}
	} else {
		/* Default gateway */
		ETHERNET.gateway[0] = GW_ADDR0;
		ETHERNET.gateway[1] = GW_ADDR1;
		ETHERNET.gateway[2] = GW_ADDR2;
		ETHERNET.gateway[3] = GW_ADDR3;
	}
	
	/* Netmask */
	if (netmask != NULL) {
		/* User selected netmask */
		for (i = 0; i < 4; i++) {
			ETHERNET.netmask[i] = netmask[i];
		}
	} else {
		/* Default netmask */
		ETHERNET.netmask[0] = NETMASK_ADDR0;
		ETHERNET.netmask[1] = NETMASK_ADDR1;
		ETHERNET.netmask[2] = NETMASK_ADDR2;
		ETHERNET.netmask[3] = NETMASK_ADDR3;
	}
	
	/* IP is not set yet */
	ETHERNET.ip_is_set = 0;
	
	/* Link is up */
	ETHERNET.link_is_up = 1;
	
	/* Server is disabled by default */
	ETHERNET.server_active = 0;
	
	/* Set speed and duplex options */
	ETHERNET.speed_100m = 0;
	ETHERNET.full_duplex = 0;
	
	/* Set default values */
	ETHERNET.Client_RX_Bytes = 0;
	ETHERNET.Client_TX_Bytes = 0;
	ETHERNET.Server_RX_Bytes = 0;
	ETHERNET.Server_TX_Bytes = 0;
	ETHERNET.Server_Connections = 0;
	ETHERNET.server_port = 0;
	ETHERNET.timeout_detected = 0;
	
	/* Configure ethernet (GPIOs, clocks, MAC, DMA) */
	if (!ETH_BSP_Config()) {
		/* Error */
		result = ETHERNET_Result_Error;
	}
	
	/* Initialize the LwIP stack */
	LwIP_Init();
	
	/* We are initialized now */
	Initialized = 1;
	
	/* Return result */
	return result;
}

ETHERNET_Result_t ETHERNETSERVER_Enable(uint16_t port) {
	if (!Initialized) {
		/* If not initialized, return error */
		return ETHERNET_Result_Error;
	}
	
	/* Server is active */
	if (ETHERNET.server_active) {
		/* Return OK */
		return ETHERNET_Result_Ok;
	}
	
	/* Only first time it will work */
	if (ETHERNET.server_port == 0) {
		/* Use user selectable server port number */
		if (port != 0) {
			ETHERNET.server_port = port;
		} else {
			/* Use default port */
			ETHERNET.server_port = ETHERNET_SERVER_PORT;
		}
	}
	
	/* Httpd init */
	httpd_init();
	
	/* Server is active */
	ETHERNET.server_active = 1;
	
	/* Return Ok */
	return ETHERNET_Result_Ok;
}

ETHERNET_Result_t ETHERNETSERVER_Disable(void) {
	if (!Initialized) {
		/* If not initialized, return error */
		return ETHERNET_Result_Error;
	}
		
	/* Server is disabled */
	if (!ETHERNET.server_active) {
		/* Return OK */
		return ETHERNET_Result_Ok;
	}
	
	/* Httpd init */
	httpd_deinit();
	
	/* Server is active */
	ETHERNET.server_active = 0;
	
	/* Return Ok */
	return ETHERNET_Result_Ok;
}

ETHERNET_Result_t ETHERNET_Update(void) {
	/* Check if any packet received */
	if (LwIP_CheckFrameReceived()) { 
		/* Process received ethernet packet */
		LwIP_Pkt_Handle();
	}
	
	/* Handle periodic timers for LwIP */
	LwIP_Periodic_Handle(EthernetLocalTime);
	
	/* Return OK */
	return ETHERNET_Result_Ok;
}

ETHERNET_Result_t ETHERNET_TimeUpdate(uint16_t millis) {
	/* Increase time by number of millis */
	EthernetLocalTime += millis;
	
	/* Return OK */
	return ETHERNET_Result_Ok;
}

ETHERNET_Result_t ETHERNET_TestReady(void) {
	/* Return ok if we have ip set and cable is plugged in */
	if (
		ETHERNET.ip_is_set &&
		ETHERNET.link_is_up
	) {
		/* Return OK */
		return ETHERNET_Result_Ok;
	}
	
	/* Return error */
	return ETHERNET_Result_Error;
}

ETHERNET_Result_t ETHERNETCLIENT_Connect(char* conn_name, uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port, void* user_parameters) {
	err_t e;
	
	/* If library is not initialized */
	if (!Initialized) {
		return ETHERNET_Result_LibraryNotInitialized;
	}
	
	/* If timeout detected flag is set */
	if (ETHERNET.timeout_detected) {
		/* Return error */
		return ETHERNET_Result_Error;
	}
	
	/* Check if link is up */
	if (!ETHERNET.link_is_up) {		
		/* Return error */
		return ETHERNET_Result_LinkIsDown;
	}
	
	/* If we don't have IP yet */
	if (!ETHERNET.ip_is_set) {
		/* Return error */
		return ETHERNET_Result_IPIsNotSetYet;
	}
	
	/* Try to Connect */
	e = tcp_echoclient_connect(conn_name, ip1, ip2, ip3, ip4, port, user_parameters);
	if (e == ERR_RST) {
		/* We need system reset here */
		/* If user want's to implement it, otherwise problems can happen */
		ETHERNET_SystemResetCallback();
		/* We need to make hard reset */
		return ETHERNET_Result_NeedHardReset;
	} else if (e != ERR_OK) {
		/* Return error */
		return ETHERNET_Result_Error;
	}
	
	/* Return OK */
	return ETHERNET_Result_Ok;
}

ETHERNET_Result_t ETHERNETSERVER_SetSSITags(ETHERNET_SSI_t* SSI_Tags, uint16_t number_of_tags) {
	static uint8_t isSet = 0;
	if (!isSet) {
		/* Set HTTP SSI Handler */
		http_set_ssi_handler(ETHERNETSERVER_SSICallback, (char const **)SSI_Tags, number_of_tags);
		
		/* Set */
		isSet = 1;
		
		/* Return OK */
		return ETHERNET_Result_Ok;
	}
	
	/* Return error */
	return ETHERNET_Result_Error;
}

ETHERNET_Result_t ETHERNETSERVER_SetCGIHandlers(const ETHERNET_CGI_t *cgis, uint16_t number_of_handlers) {
	static uint8_t isSet = 0;
	if (!isSet) {
		/* Set HTTP CGI Handlers */
		http_set_cgi_handlers((tCGI *)cgis, number_of_handlers);
		
		/* Set */
		isSet = 1;
		
		/* Return OK */
		return ETHERNET_Result_Ok;
	}
	
	/* Return error */
	return ETHERNET_Result_Error;
}

ETHERNET_Result_t ETHERNETDNS_GetHostByName(char* host_name) {
	struct ip_addr ip_addr;
	
	/* Check if we are ready */
	if (
		ETHERNET_TestReady() != ETHERNET_Result_Ok ||
		DNS_Working
	) {
		/* Return error */
		return ETHERNET_Result_Error;
	}
	
	/* Test response */
	switch (dns_gethostbyname(host_name, &ip_addr, ETHERNET_INT_DNSFoundCallback, NULL)) {
		case ERR_OK:
			/* We have valid IP, call user function */
#if BYTE_ORDER == BIG_ENDIAN
			ETHERNETDNS_FoundCallback(host_name, (ip_addr.addr >> 24) & 0xFF, (ip_addr.addr >> 16) & 0xFF, (ip_addr.addr >> 8) & 0xFF, (ip_addr.addr >> 24) & 0xFF);
#else
			ETHERNETDNS_FoundCallback(host_name, (ip_addr.addr >> 0) & 0xFF, (ip_addr.addr >> 8) & 0xFF, (ip_addr.addr >> 16) & 0xFF, (ip_addr.addr >> 24) & 0xFF);
#endif
			/* Return Ok */
			return ETHERNET_Result_Ok;
		case ERR_INPROGRESS:
			/* Set flag */
			DNS_Working = 1;
			/* Return Ok */
			return ETHERNET_Result_Ok;
		default:	
			/* We don't have valid IP, call user function */
			ETHERNETDNS_ErrorCallback((char *)host_name);
		
			/* Return error */
			break;
	}
	
	/* Return error */
	return ETHERNET_Result_Error;
}

/* CALLBACKS */
/* If functions needs to be changed, then user should make them in custom file */

/* Common callbacks */
__weak void ETHERNET_LinkIsDownCallback(void) {

}

__weak void ETHERNET_LinkIsUpCallback(void) {

}

__weak void ETHERNET_IPIsSetCallback(uint8_t ip_addr1, uint8_t ip_addr2, uint8_t ip_addr3, uint8_t ip_addr4, uint8_t dhcp) {

}

__weak void ETHERNET_DHCPStartCallback(void) {

}

__weak void ETHERNET_SystemResetCallback(void) {

}
/* Client based callbacks */
__weak uint16_t ETHERNETCLIENT_CreateHeadersCallback(TCPCLIENT_t* connection, char* buffer, uint16_t buffer_length) {
	strcpy((char *)buffer, "GET / HTTP/1.1\r\n");
	sprintf((char *)buffer, "%sHost: %s\r\n", buffer, connection->name);
	strcat((char *)buffer, "Connection: close\r\n");
	strcat((char *)buffer, "\r\n");
	
	/* Return number of characters */
	return strlen(buffer);
}

__weak void ETHERNETCLIENT_ConnectionStartedCallback(TCPCLIENT_t* connection) {
	/* Do nothing by default */
}

__weak void ETHERNETCLIENT_ConnectedCallback(TCPCLIENT_t* connection) {
	/* Do nothing by default */
}

__weak void ETHERNETCLIENT_ConnectionClosedCallback(TCPCLIENT_t* connection, uint8_t success) {
	/* Do nothing */
	
	/* User can do stuff here when connection is closed */
	/* For example, here you can make a new connection */
}

__weak void ETHERNETCLIENT_ErrorCallback(TCPCLIENT_t* connection) {

}

__weak void ETHERNETCLIENT_ReceiveDataCallback(TCPCLIENT_t* connection, uint8_t* buffer, uint16_t buffer_length, uint16_t total_length) {

}

/* Server based callbacks */
__weak uint16_t ETHERNETSERVER_SSICallback(int iIndex, char* pcInsert, int iInsertLen) {
	/* Return 0 */
	return 0;
}

__weak uint8_t ETHERNETSERVER_ClientConnectedCallback(struct tcp_pcb* pcb) {
	/* Return 1, allow connection */
	return 1;
}

__weak void ETHERNETSERVER_ClientDisconnectedCallback(void) {
	/* Do nothing */
}

__weak int ETHERNETSERVER_OpenFileCallback(struct fs_file* file, const char* name) {
	/* Return 0, custom user file is not opened */
	return 0;
}

__weak int ETHERNETSERVER_ReadFileCallback(struct fs_file* file, char *buffer, int count) {
	/* Return -1 (End Of File), we read 0 characters */
	return -1;
}

__weak void ETHERNETSERVER_CloseFileCallback(struct fs_file* file) {
	/* Do nothing */
}

/* Server POST based callbacks */
__weak uint8_t ETHERNETSERVER_PostRequestBeginCallback(ETHERNETPOST_t* params) {
	return 0;
}

__weak uint8_t ETHERNETSERVER_PostRequestReceiveDataCallback(ETHERNETPOST_t* params, struct pbuf* p) {
	return 0;
}

__weak void ETHERNETSERVER_PostRequestEndCallback(void* connection, char* response_uri, u16_t response_uri_len) {

}

/* DNS based callbacks */
__weak void ETHERNETDNS_FoundCallback(char* host_name, uint8_t ip_addr1, uint8_t ip_addr2, uint8_t ip_addr3, uint8_t ip_addr4) {

}

__weak void ETHERNETDNS_ErrorCallback(char* host_name) {

}

/* PHY based callbacks */
__weak void ETHERNETPHY_CustomOptions(uint32_t PHYAddress) {
	/* Called when initialization for ethernet is done */
	/* User can here do custom initialization settings, based on custom PHY */
	
	/* Change LED mode on DP83848 for example */
/*
	uint32_t Reg_Val;
	
	//Read register 0x19 on DP83848
	Reg_Val = ETH_ReadPHYRegister(PHYAddress, 0x19);
	
	//Clear both LED bits, SET led mode to 2, read DP83848 datasheet for more info
	Reg_Val &= ~(1 << 5 | 1 << 6);
	
	//Write new value to register
	ETH_WritePHYRegister(PHYAddress, 0x19, Reg_Val);
*/
}

__weak uint8_t ETHERNET_InitPinsCallback(void) {
	/* Return 0, use default pins */
	return 0;
}


/* Internal functions */
void ETHERNET_INT_CustomOptions(uint32_t PHYAddress) {	
	/* Call user functions */
	ETHERNETPHY_CustomOptions(PHYAddress);
}

/* Called when IP is set */
void ETHERNET_INT_SetIPAddress(uint8_t ip_addr1, uint8_t ip_addr2, uint8_t ip_addr3, uint8_t ip_addr4, uint8_t dhcp) {
	/* If we get IP address from DHCP, then this is not static address */
	if (dhcp) {
		/* We don't have static ip */
		ETHERNET.staticip = 0;
	}
	
	/* Fill IP address */
	ETHERNET.ip_addr[0] = ip_addr1;
	ETHERNET.ip_addr[1] = ip_addr2;
	ETHERNET.ip_addr[2] = ip_addr3;
	ETHERNET.ip_addr[3] = ip_addr4;
	
	/* Set variable to detect we are ready to connect */
	ETHERNET.ip_is_set = 1;
	
	/* Set flag */
	ETHERNET.link_is_up = 1;
	
	/* Call user function */
	ETHERNET_IPIsSetCallback(ip_addr1, ip_addr2, ip_addr3, ip_addr4, dhcp);
}

/* Called when cable is plugged back */
void ETHERNET_INT_LinkIsUpCallback(void) {
	/* Set flag */
	ETHERNET.link_is_up = 1;
	
	/* Waiting for IP */
	ETHERNET.ip_is_set = 0;
	
	/* Call TCP echoclient callback */
	tcp_echoclient_link_is_up_callback();
	
	/* Call user function */
	ETHERNET_LinkIsUpCallback();
}

/* Called when cable is detected to be unplugged */
void ETHERNET_INT_LinkIsDownCallback(void) {
	/* Set flag */
	ETHERNET.link_is_up = 0;
	
	/* Call TCP echoclient callback */
	tcp_echoclient_link_is_down_callback();
	
	/* Call user function */
	ETHERNET_LinkIsDownCallback();
}

/* Called when dns_geethostbyname finished */
void ETHERNET_INT_DNSFoundCallback(const char *name, struct ip_addr *ipaddr, void *callback_arg) {
	/* Not working anymore */
	DNS_Working = 0;
	/* Check for IP */
	if ((ipaddr) && (ipaddr->addr)) {
#if BYTE_ORDER == BIG_ENDIAN
		/* We have valid IP, call user function */
		ETHERNETDNS_FoundCallback((char *)name, (ipaddr->addr >> 24) & 0xFF, (ipaddr->addr >> 16) & 0xFF, (ipaddr->addr >> 8) & 0xFF, (ipaddr->addr >> 0) & 0xFF);
#else
		/* We have valid IP, call user function */
		ETHERNETDNS_FoundCallback((char *)name, (ipaddr->addr >> 0) & 0xFF, (ipaddr->addr >> 8) & 0xFF, (ipaddr->addr >> 16) & 0xFF, (ipaddr->addr >> 24) & 0xFF);	
#endif /* Little endian order */
	} else {
		/* We don't have valid IP, call user function */
		ETHERNETDNS_ErrorCallback((char *)name);
	}
}
