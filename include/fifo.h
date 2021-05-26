#ifndef INCLUDE_FIFO
#define INCLUDE_FIFO

#define LEN(x) (sizeof(x) / sizeof(x[0]))

#define FIFO_VAR_NAME(a, b) a##b
#define FIFO_READ_IDX(fifo) FIFO_VAR_NAME(fifo, _read)
#define FIFO_WRITE_IDX(fifo) FIFO_VAR_NAME(fifo, _write)
#define FIFO_LEVEL(fifo) FIFO_VAR_NAME(fifo, _level)
#define FIFO_BUFFER(fifo) FIFO_VAR_NAME(fifo, _buffer)
#define FIFO_AT_WRITE(fifo) FIFO_BUFFER(fifo)[FIFO_WRITE_IDX(fifo)]
#define FIFO_AT_READ(fifo) FIFO_BUFFER(fifo)[FIFO_READ_IDX(fifo)]
#define FIFO_SIZE(fifo) (sizeof(fifo##_buffer) / sizeof(fifo##_buffer[0]))
#define fifo_create(fifo, size)                                        \
    __xdata uint8_t FIFO_BUFFER(fifo)[size];                           \
    volatile __data uint8_t FIFO_WRITE_IDX(fifo), FIFO_READ_IDX(fifo), \
        FIFO_LEVEL(fifo)

#define fifo_declare_extern(fifo, size)                                       \
    extern __xdata uint8_t FIFO_BUFFER(fifo)[size];                           \
    extern volatile __data uint8_t FIFO_WRITE_IDX(fifo), FIFO_READ_IDX(fifo), \
        FIFO_LEVEL(fifo)

#define fifo_init(fifo)           \
    do {                          \
        FIFO_READ_IDX(fifo) = 0;  \
        FIFO_WRITE_IDX(fifo) = 0; \
        FIFO_LEVEL(fifo) = 0;     \
    } while (0)
#define fifo_is_empty(fifo) (FIFO_LEVEL(fifo) == 0)
#define fifo_not_empty(fifo) (FIFO_LEVEL(fifo) != 0)
#define fifo_is_full(fifo) (FIFO_LEVEL(fifo) > fifo_mask(fifo))
#define fifo_not_full(fifo) (FIFO_LEVEL(fifo) <= fifo_mask(fifo))
#define fifo_mask(fifo) ((uint8_t)(FIFO_SIZE(fifo) - 1))
#define fifo_available_mem(fifo) (FIFO_SIZE(fifo) - FIFO_LEVEL(fifo))
#define fifo_available_data(fifo) (FIFO_LEVEL(fifo))
#define fifo_wrap(fifo, index) ((uint8_t)(index)&fifo_mask(fifo))
// write without checking for buffer overflow
#define fifo_write_unsave(fifo, data)                                       \
    do {                                                                    \
        FIFO_AT_WRITE(fifo) = data;                                         \
        FIFO_WRITE_IDX(fifo) = fifo_wrap(fifo, (FIFO_WRITE_IDX(fifo) + 1)); \
        FIFO_LEVEL(fifo)++;                                                 \
    } while (0)
// write a byte to the FIFO if space is available
#define fifo_write(fifo, data)             \
    do {                                   \
        if (fifo_not_full(fifo)) {         \
            fifo_write_unsave(fifo, data); \
        }                                  \
    } while (0)

#define fifo_read_unsave(fifo, data)                                      \
    do {                                                                  \
        data = FIFO_AT_READ(fifo);                                        \
        FIFO_READ_IDX(fifo) = fifo_wrap(fifo, (FIFO_READ_IDX(fifo) + 1)); \
        FIFO_LEVEL(fifo)--;                                               \
    } while (0)

#define fifo_read(fifo, data)             \
    do {                                  \
        if (fifo_not_empty(fifo)) {       \
            fifo_read_unsave(fifo, data); \
        }                                 \
    } while (0)

#define fifo_n_until_writewrap(fifo) (FIFO_SIZE(fifo) - FIFO_WRITE_IDX(fifo))

#define fifo_n_until_readwrap(fifo) (FIFO_SIZE(fifo) - FIFO_READ_IDX(fifo))

#define fifo_remove_unsave(fifo, n)                                     \
    do {                                                                \
        FIFO_LEVEL(fifo) -= n;                                          \
        FIFO_READ_IDX(fifo) = fifo_wrap(fifo, (FIFO_READ_IDX(fifo) + n)); \
    } while (0)

#define fifo_remove(fifo, n)         \
    do {                             \
        if (n > FIFO_LEVEL(fifo)) {  \
            n = FIFO_LEVEL(fifo);    \
        }                            \
        fifo_remove_unsave(fifo, n); \
    } while (0)

#define fifo_reset(fifo)          \
    do {                          \
        FIFO_READ_IDX(fifo) = 0;  \
        FIFO_WRITE_IDX(fifo) = 0; \
        FIFO_LEVEL(fifo) = 0;     \
    } while (0)

#define fifo_peek(fifo, x) \
    FIFO_BUFFER(fifo)[fifo_wrap(fifo, (FIFO_READ_IDX(fifo) + x))]

#endif /* INCLUDE_FIFO */
