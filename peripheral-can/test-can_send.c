/* 1.报文发送程序 */ 
 #include <stdio.h> 
 #include <stdlib.h> 
 #include <string.h> 
 #include <unistd.h> 
 #include <net/if.h> 
 #include <sys/ioctl.h> 
 #include <sys/socket.h> 
 #include <linux/can.h> 
 #include <linux/can/raw.h> 
 
 
 /* 将CAN0波特率设置为500000 bps */
#define ip_cmd_set_can_params  "ip link set can0 type can bitrate 500000 triple-sampling on"
 
 /* 打开CAN0 */
#define ip_cmd_open            "ifconfig can0 up"     
 
 /* 关闭CAN0 */    
#define ip_cmd_close           "ifconfig can0 down"   



 int main() 
 { 
 	int fd, nbytes; 
	struct sockaddr_can addr; 
	struct ifreq ifr; 
	struct can_frame frame[2] = {{0}}; 
	
	system(ip_cmd_close);               
    system(ip_cmd_set_can_params);
	system(ip_cmd_open);

	fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);//创建套接字 
	strcpy(ifr.ifr_name, "can0" ); 
	ioctl(fd, SIOCGIFINDEX, &ifr); //指定 can0 设备 
	addr.can_family = AF_CAN; 
	addr.can_ifindex = ifr.ifr_ifindex; 
	bind(fd, (struct sockaddr *)&addr, sizeof(addr));//将套接字与 can0 绑定 
	
	//禁用过滤规则，本进程不接收报文，只负责发送 
	setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0); 
	
	//生成两个报文 
	frame[0].can_id = 0x11; 
	frame[0]. can_dlc = 1; 
	frame[0].data[0] = 'Y'; 
	frame[1].can_id = 0x22; 
	frame[1]. can_dlc = 1; 
	frame[1].data[0] = 'N'; 
	//循环发送两个报文 
	while(1) 
	{ 
		nbytes = write(fd, &frame[0], sizeof(frame[0])); //发送 frame[0] 
        printf("write ret:%d",nbytes);
		if(nbytes != sizeof(frame[0])) 
		{ 
			printf("Send Error frame[0]\n!"); 
			break; //发送错误，退出 
	 	} 
		 sleep(1); 
		 
		nbytes = write(fd, &frame[1], sizeof(frame[1])); //发送 frame[1] 
		if(nbytes != sizeof(frame[1])) 
		{ 
			printf("Send Error frame[1]\n!"); 
			break; 
	 	} 
		sleep(1); 
	} 
	 close(fd); 
	 return 0; 
} 