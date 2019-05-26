extern void rtc_set_now(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t,uint8_t,uint16_t);
extern void rtc_get_now(uint8_t *,uint8_t *,uint8_t *,uint8_t *,uint8_t *,uint8_t *,uint16_t *);
extern void rtc_write(uint8_t,uint8_t);

#define mask_data 0x80
#define mask_clk 0x40
#define mask_rst 0x10
#define mask_rd 0x20

#define ds1302_port 0xc0

