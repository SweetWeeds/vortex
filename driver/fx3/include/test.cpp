
#include "usbXfer.h"


namespace chip
{
	usbxfer::usbpipe Device;
	void open_chip(void){
		if(Device.open() == false){
			printf("open error \n");
		}
	}
}

int main(){
	
	chip::open_chip();
	printf("Open Chip!!\n");
	unsigned char buffer[BUFFER_SIZE];
	unsigned char out_buf[BUFFER_SIZE];
	for(int i=0;i<20;i++){
		buffer[0] = i;

		//1. Write Byte Array
		chip::Device.write_byte(buffer, 0, BUFFER_SIZE);


		//2.1 Read Head
		chip::Device.read_byte(out_buf, 0, BUFFER_SIZE);

		//2.2 Read Byte Array
		chip::Device.read_byte(out_buf, 0, BUFFER_SIZE);
		printf("%d ", out_buf[0]);

		//2.3 Read Tail
		chip::Device.read_byte(out_buf, 0, BUFFER_SIZE);
	}

	return 0;
}

