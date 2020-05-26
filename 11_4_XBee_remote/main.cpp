#include "mbed.h"

RawSerial pc(USBTX, USBRX);
RawSerial xbee(D12, D11);

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void xbee_rx_interrupt(void);
void xbee_rx(void);
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);

int main(){
    pc.baud(9600);

    char xbee_reply[4];

    // XBee setting
    xbee.baud(9600);
    // send the setting mode command to xbee
    xbee.printf("+++");
    xbee_reply[0] = xbee.getc();
    xbee_reply[1] = xbee.getc();

    if (xbee_reply[0] == 'O' && xbee_reply[1] == 'K') {
        pc.printf("enter AT mode.\r\n");
	// reset the xbee_reply buffer to '\0'
        xbee_reply[0] = '\0';
        xbee_reply[1] = '\0';
    }
//------------------------------------------------------------------
// set the parameter
    xbee.printf("ATMY 0x240\r\n");
    reply_messange(xbee_reply, "setting MY : 0x240");

    xbee.printf("ATDL  0x140\r\n");
    reply_messange(xbee_reply, "setting DL : 0x140");

    xbee.printf("ATID 0x1\r\n");
    reply_messange(xbee_reply, "setting PAN ID : 0x1");

    xbee.printf("ATWR\r\n");
    reply_messange(xbee_reply, "write config");
//------------------------------------------------------------------------
// check if we set the parameter correctely
    xbee.printf("ATMY\r\n");
    check_addr(xbee_reply, "MY");

    xbee.printf("ATDL\r\n");
    check_addr(xbee_reply, "DL");

    xbee.printf("ATCN\r\n");
    reply_messange(xbee_reply, "exit AT mode");
    xbee.getc();

    // start
    pc.printf("start\r\n");
    t.start(callback(&queue, &EventQueue::dispatch_forever));

    // Setup a serial interrupt function to receive data from xbee
    xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
}


void xbee_rx_interrupt(void)
{
    xbee.attach(NULL, Serial::RxIrq); // detach interrupt
 
    queue.call(&xbee_rx);
}

void xbee_rx(void)
{
    static int i = 0;
    static char buf[100] = {0};
    
    // if we get something from the xbee(send from python xbee to k66f xbee), 
    // we will print it out
    while (xbee.readable()) {
        char c = xbee.getc();
	pc.printf("c = %d i = %d \r\n", c, i);
        if(c != '\r' && c!= '\n') {
            buf[i] = c;
            i++;
            buf[i] = '\0';
        }
        else {
	    i = 0;
            pc.printf("Get: %s\r\n------\r\n", buf);
            xbee.printf("%s", buf);
        }
    }
    wait(0.1);
    xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt
}

void reply_messange(char *xbee_reply, char *messange){
    // get 3 char, since the reply is at most 3 char
    // and store them in the xbee_reply, which is a char* in main
    xbee_reply[0] = xbee.getc();
    xbee_reply[1] = xbee.getc();
    xbee_reply[2] = xbee.getc();
   
    // if the reply is "OK", then clear the buffer
    // and we will print the pre-defined mission messange from main()
    if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K') {
        pc.printf("%s\r\n", messange);
        xbee_reply[0] = '\0';
        xbee_reply[1] = '\0';
        xbee_reply[2] = '\0';
  }
}

void check_addr(char *xbee_reply, char *messenger){
    xbee_reply[0] = xbee.getc();
    xbee_reply[1] = xbee.getc();
    xbee_reply[2] = xbee.getc();
    xbee_reply[3] = xbee.getc();
    // print the address out! Then, clear the buffer
    pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    xbee_reply[2] = '\0';
    xbee_reply[3] = '\0';
}
