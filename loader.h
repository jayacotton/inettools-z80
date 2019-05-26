
//
// this data is the first 4 bytes to the driver
//
struct driver_file {
	uint8_t		*dest_addr;	// testination address 
	uint16_t	size;		// driver size in bytes
};

//
// so just take the load address and cast it as a struct driver_head *
// and you get access to these items.
//
struct driver_head{
	uint8_t		pad[3];		// bdos link jump
	uint32_t	*counter;	// pointer to 32bit counter
	uint16_t	*link;		// pointer to bdos linkup addres
	uint16_t	*ctcint;	// pointer to actual interupt handler
	uint16_t	*chain;		// interupt chain address
/* the following pointers are available to the driver to provide
service to. */
	uint16_t	*p0;
	uint16_t	*p1;
	uint16_t	*p2;
	uint16_t	*p3;
	uint16_t	*p4;
	uint16_t	*p5;
	uint16_t	*p6;
	uint16_t	*p7;
	uint16_t	*p8;
	uint16_t	*p9;
};
extern uint8_t loader(uint8_t *,uint8_t *);
extern uint16_t *GetServ();
extern uint32_t *GetCount();
