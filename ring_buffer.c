#include <stdio.h>
#include <stdint.h>

#define BUFFER_SIZE 8

typedef struct
{
    uint8_t buffer[BUFFER_SIZE];
    int head;
    int tail;
    int count;
} RingBuffer;

// Initialize buffer 
void rb_init(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

int rb_is_full(RingBuffer *rb) //check full
{
    return (rb->count == BUFFER_SIZE);
}

int rb_is_empty(RingBuffer *rb)//check empty
{
    return (rb->count == 0);
}

int rb_count(RingBuffer *rb) //get count
{
    return rb->count;
}

int rb_write(RingBuffer *rb, uint8_t data)//write one bye
{
    if (rb_is_full(rb))
    {
        return 0;   /* FAIL */
    }

    rb->buffer[rb->head] = data;

    /*
 * Using '& (BUFFER_SIZE - 1)' instead of '% BUFFER_SIZE'.
 * Bitwise AND is faster than modulo on many MCUs because it
 * avoids a division operation. This optimization works only
 * when BUFFER_SIZE is a power of 2 (2, 4, 8, 16, ...).
 */
rb->head = (rb->head + 1) & (BUFFER_SIZE - 1);
    rb->head = (rb->head + 1) & (BUFFER_SIZE - 1);

    rb->count++;

    return 1;       /* SUCCESS */
}

/*
 * Faster wrap-around using bitwise AND.
 * Valid only when BUFFER_SIZE is a power of 2.
 */
int rb_read(RingBuffer *rb, uint8_t *data)
{
    if (rb_is_empty(rb))
    {
        return 0;   /* FAIL */
    }

    *data = rb->buffer[rb->tail];

    rb->tail = (rb->tail + 1) & (BUFFER_SIZE - 1);

    rb->count--;

    return 1;       /* SUCCESS */
}

int main()
{
    RingBuffer rb;
    uint8_t data;

    rb_init(&rb);

    uint8_t first_data[] =
    {
        0x41,0x42,0x43,0x44,
        0x45,0x46,0x47,0x48
    };

    for(int i=0;i<8;i++)
    {
        if(rb_write(&rb, first_data[i]))
        {
            printf("[WRITE] 0x%02X -> OK (count=%d)",
                   first_data[i], rb_count(&rb));

            if(rb_is_full(&rb))
                printf(" FULL");

            printf("\n");
        }
    }

    if(!rb_write(&rb, 0x99))
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

    for(int i=0;i<3;i++)
    {
        if(rb_read(&rb, &data))
        {
            printf("[READ] -> 0x%02X (count=%d)\n",
                   data, rb_count(&rb));
        }
    }

    uint8_t new_data[] = {0x49,0x4A,0x4B};

    for(int i=0;i<3;i++)
    {
        if(rb_write(&rb, new_data[i]))
        {
            printf("[WRITE] 0x%02X -> OK (count=%d)\n",
                   new_data[i], rb_count(&rb));
        }
    }

    while(!rb_is_empty(&rb))
    {
        rb_read(&rb, &data);

        printf("[READ] -> 0x%02X (count=%d)\n",
               data, rb_count(&rb));
    }

    if(!rb_read(&rb, &data))
    {
        printf("[READ] (empty) -> FAIL (buffer empty)\n");
    }

    return 0;
}
