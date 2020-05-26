#include "mbed.h"

RawSerial pc(USBTX, USBRX); // == screen()
RawSerial xbee(D12, D11); // == the board
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void pc_rx_interrupt(void);
void pc_rx(void);

void xbee_rx_interrupt(void);
void xbee_rx(void);

int main()
{ 
    pc.baud(9600); // since we use uart to transmit information, we need baudrate 
    xbee.baud(9600); 
    pc.printf("Begin test\r\n"); 
    t.start(callback(&queue, &EventQueue::dispatch_forever));

    // Setup a serial interrupt function of receiving data from pc
    pc.attach(pc_rx_interrupt, Serial::RxIrq);
    //!!!whenever there is data transmit to pc, it will cause a interrupt!!!
    
    // Setup a serial interrupt function of receiving data from xbee
    xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
}

//-------------------------------------------------------------------
void pc_rx_interrupt(void)
{
    pc.attach(NULL, Serial::RxIrq);
    // use attach again will cover the last command
    // so there is no more interrupt until the pc_rx is finished
    queue.call(&pc_rx);
}

void pc_rx(void)
{
    // maybe because the function is usually called
    // thus we declare them as static
    static int i = 0;
    static char str[50] = {0};  // serial buffer    

    while (pc.readable()) {
	// keep getting new character one by one
        char c = pc.getc();
        
	// if there is no enter, then hold, not send
	if (c != '\r' && c!= '\n') {
	    // put what we excactly type on the screen
            pc.putc(c);
            str[i] = c;
            i++;
            str[i] = '\0';
	    // if there is exactly +++ then don't have to wait for enter, 
	    // directely send it to xbee
            if (strncmp(str, "+++", 3) == 0) {
                xbee.printf("+++");
                i = 0;
		// reset i to 0
            }
        }
	// detect an enter, send the string to xbee
        else {
            i = 0;
	    // reset i to 0
            xbee.printf("%s\r\n", str);
	    // send the string to bee
            pc.printf("\r\n", str);
	    // print an enter on the screen
        }
    }
    wait(0.001);
    pc.attach(pc_rx_interrupt, Serial::RxIrq); // reattach interrupt
}
//------------------------------------------------------------------
void xbee_rx_interrupt(void)
{
    xbee.attach(NULL, Serial::RxIrq); // detach interrupt
    queue.call(&xbee_rx);
}

void xbee_rx(void)
{
    // print whatever xbee send to k66f and print it out
    while (xbee.readable()) {
        char c = xbee.getc();
        if (c != '\r' && c != '\n') {
            pc.printf("%c",c);
        }
        else {
            pc.printf("\r\n");
        }
    }
    wait(0.001);
    xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt
}
