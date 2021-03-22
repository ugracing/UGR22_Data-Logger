#include <stdio.h>
#include <string.h>
#include <stdint.h>

int gps_msg_config(const char *nema, int rus1){

	if (strlen(nema) != 3) return 0;
	int epochrus1 = 1000/rus1;

	char msg[32];
	snprintf(msg, sizeof(msg)-1,"PUBX,40,%s,%d,0,0,0,0,0", nema, epochrus1);

	int checksum_int = 0;
	for (int i = 0; msg[i] && i < 32; i++)
		checksum_int ^= (unsigned char)msg[i];

	char checksum[8];
	snprintf(checksum, sizeof(checksum)-1, "*%.2X", checksum_int);

	char messagetosend[40];
	snprintf(messagetosend, sizeof(messagetosend), "$%s%s\r\n", msg, checksum);
	printf("%s", messagetosend);
	return 1;
}

int main(){
	gps_msg_config("GLL", 1000);
}