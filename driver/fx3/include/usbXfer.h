/*
*  Init. Hand_Tracking (2018 ISSCC Demo)
*  Rev. 2019 by J.S. Lee (for LNPU Board, USB Read/Write Operation)
*  Rev. 2022 by H.K. Kwon (for Linux, USB )
*/

#ifndef __USBXFER_H__
#define __USBXFER_H__
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./cyusb.h"

#define VID_USB 0x04b4
#define PID_USB 0x00f1
#define EP_OUT  0x01
#define EP_IN   0x81
#define BUFFER_SIZE 16384
#define BUFFER_SIZE_4 4096
#define TIMEOUT 100

typedef char CHAR;
typedef short SHORT;
typedef long LONG;

namespace usbxfer
{
	class usbpipe
	{
	private:
		//OVERLAPPED outOvLap, inOvLap;
		//CCyBulkEndPoint *BulkInEpt;
		//CCyBulkEndPoint *BulkOutEpt;
		//CCyUSBDevice *Device;
        cyusb_handle* p_handler;
		unsigned char outbuffer[BUFFER_SIZE];
		unsigned char inbuffer[BUFFER_SIZE];
	public:
		usbpipe()
		{
            printf("[INFO:usbXfer:usbpipe:new inst] New Instance\n");
            this->p_handler = NULL;
			//Device = NULL;
			//BulkInEpt = NULL;
			//BulkOutEpt = NULL;
		}
		bool open()
		{
            // Device Open
            int r = cyusb_open(VID_USB, PID_USB);
            if ( r < 0 ) {
                printf("[INFO:usbXfer:usbpipe:open] Failed to opening library\n");
                return false;
            } else if ( r == 0 ) {
                printf("[INFO:usbXfer:usbpipe:open] No Device Found\n");
                return false;
            }
            // Get Handler
            this->p_handler = cyusb_gethandle(0);
            r = libusb_kernel_driver_active(this->p_handler, 0);
            if ( r != 0) {
                printf("[INFO:usbXfer:usbpipe:open] Kernel Driver Active. Existing\n");
                return false;
            }
            r = libusb_claim_interface(this->p_handler, 0);
            if (r != 0) {
                printf("[INFO:usbXfer:usbpipe:open] Error in claiming interface\n");
                return false;
            } else {
                printf("[INFO:usbXfer:usbpipe:open] Succesfully claimed interface\n");
                return true;
            }

            /*
			Device = new CCyUSBDevice(NULL);
			int devices = Device->DeviceCount();
			int vID, pID;
			int d = 0;
			do {
				Device->Open(d); // Open automatically calls Close() if necessary
				vID = Device->VendorID;
				pID = Device->ProductID;
				d++;
				printf("%x %x\n", vID, pID);
			} while ((d < devices) && (vID != 0x04b4) && (pID != 0x00f1));
			if ((vID != 0x04b4) || (pID != 0x00f1)) return false;
			int eptCount = Device->EndPointCount();
			if (eptCount < 3) return false;
			for (int i = 1; i<eptCount; i++) {
				bool bIn = ((Device->EndPoints[i]->Address & 0x80) == 0x80);
				bool bBulk = (Device->EndPoints[i]->Attributes == 2);
				if (bBulk && bIn) BulkInEpt = (CCyBulkEndPoint *)Device->EndPoints[i];
				if (bBulk && !bIn) BulkOutEpt = (CCyBulkEndPoint *)Device->EndPoints[i];
			}
			outOvLap.hEvent = CreateEvent(NULL, false, false, "CYUSB_OUT");
			inOvLap.hEvent = CreateEvent(NULL, false, false, "CYUSB_IN");
			return true;
            */
		}
		bool close()
		{
            cyusb_close();
			//CloseHandle(inOvLap.hEvent);
			//CloseHandle(outOvLap.hEvent);
			//delete Device;
			return true;
		}

		unsigned int write_byte(unsigned char * buffer, unsigned int base, unsigned int len) //len&base should be 4 mutiplier
		{
			unsigned int left_num = len;
			unsigned int xfer_num = 0;
			unsigned char * c_buffer = buffer; //current buffer address
			LONG ret;
            int transffered = 0;
			//int transf=0;
			while (left_num > BUFFER_SIZE)
			{
				memcpy(outbuffer, c_buffer, sizeof(unsigned char)*BUFFER_SIZE);
                cyusb_bulk_transfer(this->p_handler, EP_OUT, outbuffer, BUFFER_SIZE, &transffered, TIMEOUT);
                /*
				UCHAR *outContext = Device->BulkOutEndPt->BeginDataXfer(outbuffer, BUFFER_SIZE, &outOvLap);
				Device->BulkOutEndPt->WaitForXfer(&outOvLap, 100);
				Device->BulkOutEndPt->FinishDataXfer(outbuffer, ret, &outOvLap, outContext);
                */
				if (ret != BUFFER_SIZE) return xfer_num + ret;
				left_num -= BUFFER_SIZE;
				xfer_num += BUFFER_SIZE;
				c_buffer += BUFFER_SIZE;
			}
			if (left_num == 0) return xfer_num;
			else
			{
				memcpy(outbuffer, c_buffer, sizeof(unsigned char)*left_num);
                cyusb_bulk_transfer(this->p_handler, EP_OUT, outbuffer, BUFFER_SIZE, &transffered, TIMEOUT);
                /*
				UCHAR *outContext = Device->BulkOutEndPt->BeginDataXfer(outbuffer, BUFFER_SIZE, &outOvLap);
				Device->BulkOutEndPt->WaitForXfer(&outOvLap, 100);
				Device->BulkOutEndPt->FinishDataXfer(outbuffer, ret, &outOvLap, outContext);
                */
				if (ret != BUFFER_SIZE) return xfer_num + ret;
				xfer_num += left_num;
				return xfer_num;
			}
		}
		unsigned int read_byte(unsigned char * buffer, unsigned int base, unsigned int len) //len&base should be 4 mutiplier
		{
			unsigned int left_num = len;
			unsigned int xfer_num = 0;
			unsigned char * c_buffer = buffer; //current buffer address
			LONG ret;
            int transferred = 0;
			while (left_num > BUFFER_SIZE)
			{
                /*
				UCHAR *inContext = Device->BulkInEndPt->BeginDataXfer(c_buffer, BUFFER_SIZE, &inOvLap);
				Device->BulkInEndPt->WaitForXfer(&inOvLap, 100);
				//ret = printf("%d\n", ret);
				Device->BulkInEndPt->FinishDataXfer(c_buffer, ret, &inOvLap, inContext);
                */
               cyusb_bulk_transfer(this->p_handler, EP_IN, c_buffer, BUFFER_SIZE, &transferred, TIMEOUT);
				if (ret != BUFFER_SIZE) return xfer_num+ret;
				left_num -= BUFFER_SIZE;
				xfer_num += BUFFER_SIZE;
				c_buffer += BUFFER_SIZE;
			}
			if (left_num == 0) return xfer_num;
			else
			{
                /*
				UCHAR *inContext = Device->BulkInEndPt->BeginDataXfer(inbuffer, BUFFER_SIZE, &inOvLap);
				Device->BulkInEndPt->WaitForXfer(&inOvLap, 100);
				//ret = printf("%d\n", ret);
				Device->BulkInEndPt->FinishDataXfer(inbuffer, ret, &inOvLap, inContext);
                */
                cyusb_bulk_transfer(this->p_handler, EP_IN, inbuffer, BUFFER_SIZE, &transferred, TIMEOUT);
				if (ret != left_num)
				{
					memcpy(c_buffer, inbuffer, sizeof(unsigned char)*ret);
					return xfer_num + ret;
				}
				memcpy(c_buffer, inbuffer, sizeof(unsigned char)*left_num);
				xfer_num += left_num;
				return xfer_num;
			}
		}

	};
};

#endif
