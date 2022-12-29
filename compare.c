#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


struct lppShortAnchorPosition_s {
	float position[3];
	uint8_t hash[8];
} __attribute__((packed));

int main(int argc, char *argv[]) {

	struct lppShortAnchorPosition_s pos;
	pos.position[0] = 1.0;
	pos.position[1] = 1.0;
	pos.position[2] = 1.0;
	
	struct lppShortAnchorPosition_s *posptr = &pos;
	
	memcpy(posptr->hash, "12345678", 8);

	unsigned char *str2 = posptr->hash;
	
	int result = strcmp("12345678", str2);
	
	if (result == 0) {
		printf("yes");
	} else {
		printf("no");
	}

}