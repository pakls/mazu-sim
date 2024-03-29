#ifndef __DCMBUS_H__ 
#define __DCMBUS_H__

#include "linux_common.h"
#include "dcmbus_driver_intf.h"
#include "list.h"
#include "ringbuffer.h"
#include "config_util.h"

typedef enum _ENUM_DCMBUS_CHANNEL_TYPE {
    DCMBUS_EMPTY      = 0,
    DCMBUS_SOCKET_ETH = 0x1,
    DCMBUS_SOCKET_CAN= 0x2,
    DCMBUS_DEV_RS422 = 0x3,
    DCMBUS_NULL_CHANNEL_TYPE
}ENUM_DCMBUS_CHANNEL_TYPE;

struct channel_type_enum_t {
    int enumval;
    char type_name[16];
};


typedef enum _ENUM_DCMBUS_DIR_TYPE {
    DCMBUS_DIR_TX = 0x1,
    DCMBUS_DIR_RX = 0x2,
    DCMBUS_DIR_NONE
}ENUM_DCMBUS_DIR_TYPE;


#define CHANNEL_SPECIFIER     "STRING:16 NUMBER:1 STRING:4 STRING:16 STRING:16 NUMBER:4 NUMBER:4 NUMBER:1 NUMBER:4"
#define CHANNEL_FIELDS_NAME   "ch_name", "enable", "direction", "type", "ifname", "netport", "driver_idx", "blocking", "options"
#define CHANNEL_PRINTF_FORMAT "%16s %16d %16s %16s %16s %16d %16d %16d %16d\n"

struct channel_config {
    char ch_name[16];
    uint8_t enable;
    char direction[4]; //TX, RX, TRX
    char type[16];     //socket, devfile
    char ifname[16];
    int netport;
    int driver_idx;
    uint8_t blocking;
    uint32_t options;
}__attribute__ ((packed));

#define RING_SPECIFIER     "STRING:16 NUMBER:1 STRING:4 NUMBER:4"
#define RING_FIELDS_NAME   "rg_name", "enable", "direction", "ring_size"
#define RING_PRINTF_FORMAT "%16s %16d %16s %16d\n"

struct ring_config {
    char rg_name[16];
    uint8_t enable;
    char direction[4]; //TX, RX
    int ring_size;
}__attribute__ ((packed));


#define BIND_SPECIFIER     "STRING:16 STRING:16 STRING:4"
#define BIND_FIELDS_NAME   "RING", "CHANNEL", "Direction"
#define BIND_PRINTF_FORMAT "%16s %16s %16s\n"

struct bind_config {
    char ring[16];
    char channel[16];
    char direction[4]; //TX, RX

}__attribute__ ((packed));

struct dcmbus_header_t {
    uint32_t frame_full_size;
    void *l2frame;
};

struct dcmbus_bind_entry_t {
    struct list_head list;
    int dir;
    char channel[16];
    char ring[16];
    struct ringbuffer_t *p_data_ring;
};

struct dcmbus_channel_blk_t {
    uint8_t enable;
    uint8_t blocking;
    int type;
    char ch_name[16];
    struct list_head list;
    char ifname[16];
    int netport;
    struct dcmbus_driver_ops *drv_ops;
    void *drv_priv_data;
    struct list_head txbind_lhead;
    struct list_head rxbind_lhead;

};

struct dcmbus_ring_blk_t {
    struct list_head list;
    uint8_t enable;
    char rg_name[16];
    int ring_size;
    int dir;
    struct ringbuffer_t data_ring;
};

struct dcmbus_ctrlblk_t {
    int system_type;
    int num_rings;
    int num_channels;
    int num_binds;
    struct list_head  channel_lhead;
    struct list_head  ring_lhead;
};

#ifdef __cplusplus
extern "C" {
#endif
int dcmbus_ctrlblk_init(struct dcmbus_ctrlblk_t* D,
                        const char *path_ring,
                        const char *path_chan,
                        const char *path_bind,
                        int system_type);
int dcmbus_ctrlblk_deinit(struct dcmbus_ctrlblk_t* D);
int dcmbus_channel_rx_job(struct dcmbus_ctrlblk_t* D, const char *name, int raw_size);
int dcmbus_channel_tx_job(struct dcmbus_ctrlblk_t* D, const char *ch_name);
int dcmbus_tx_direct(struct dcmbus_ctrlblk_t* D, const char *name, void *payload, uint32_t size);

int dcmbus_ring_dequeue(struct dcmbus_ctrlblk_t* D, const char *rg_name, void *payload);
int dcmbus_ring_enqueue(struct dcmbus_ctrlblk_t* D, const char *rg_name, void *payload, uint32_t size);
#ifdef __cplusplus
}
#endif


#endif  //  __DCMBUS_H__