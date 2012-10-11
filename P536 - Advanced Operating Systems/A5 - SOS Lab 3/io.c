

#include "jval.h"
#include "dllist.h"
#include "mt.h"
#include "process.h"
#include "jrb.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

void read_console_io(void *cr_buf) {
    IO_Buffer *buf = (IO_Buffer *) cr_buf;
    int i = 0;
    while (1) {
        mt_sem_down(consoleWait);
        int c = console_read();
        if (buf->count < buf->size) {
            buf->buf[buf->tail] = c;
            buf->tail++;
            buf->tail %= buf->size; 
            buf->count++;   
            int count = 0;
            i = buf->head;
            DEBUG('c', "!!!!!!!!!!!!!!!!!reading console char %c\n", c);
            //while (count < buf->count) {
            //    DEBUG('c', "%d", buf->buf[i++]);
            //    i %= CRB_SIZE; 
            //    count++;
            //}
            mt_sem_up(buf->nelem);
        }
    }
}

void write_console_io(void *cw_buf) {
    IO_Buffer *buf = (IO_Buffer *) cw_buf;
    int i = 0;
    while (1) {
        /** Read the char from io buffer */
        mt_sem_down(buf->nelem);
        //DEBUG('c', "nelm semaphore %d\n",  mt_getval(buf->nelem));
        int c = buf->buf[buf->head];
        console_write(c);
        //DEBUG('c', "writing console char %c\n", c);
        buf->head++;
        buf->head %= buf->size;
        buf->count--;   
        mt_sem_up(buf->nempty);
        mt_sem_down(writeok);
        //sleep(1);
    }
}

int io_buffer_read(IO_Buffer *iobuf, int size, char *buf) {
    int ret = 0;
    DEBUG('c', "**************normal buffer read");
    
    /*if (iobuf->nwriters == 0 && iobuf->count == 0) {
        DEBUG('c', "rading return writers 0\n");
        return 0;
    }*/
    DEBUG('c', "reading io call head %d tail %d cout %d nwriters %d\n", iobuf->head, iobuf->tail, iobuf->count, iobuf->nwriters);
   
    mt_sem_down(iobuf->read_serialize);
    int read = 0;
    while (read < size) { 
        mt_sem_down(iobuf->nelem);
        /** Read the char from io buffer */
        int c = iobuf->buf[iobuf->head];
        DEBUG('c', "reading io char %c head %d taile %d cout %d\n", c, iobuf->head, iobuf->tail, iobuf->count);
        //DEBUG('c', "reading char %c\n", c);
        iobuf->head++;
        iobuf->head %= iobuf->size;
        iobuf->count--;   
             
        buf[read++] = c;
        ret = read;
        mt_sem_up(iobuf->nempty);
        if (c == EOF || c == 255 || c == -1) {
            ret = -1;
            break;  
        } else if (c == 10){ 
            break;  
        }       
    }   
    DEBUG('c', "done reading\n");
    mt_sem_up(iobuf->read_serialize);
    return ret;
}

int io_buffer_read_exact(IO_Buffer *iobuf, int size, char *buf) {
    DEBUG('c', "**************exact buffer read %d\n", size);
    int ret = 0;
    
    if (iobuf->nwriters == 0 && iobuf->count == 0) {
        DEBUG('e', "reading return writers 0\n");
        return 0;
    }
    DEBUG('c', "reading io call head %d tail %d cout %d nwriters %d\n", iobuf->head, iobuf->tail, iobuf->count, iobuf->nwriters);
   
    mt_sem_down(iobuf->read_serialize);
    int read = 0;
    while (read < size) { 
        mt_sem_down(iobuf->nelem);
        /** Read the char from io buffer */
        int c = iobuf->buf[iobuf->head];
        DEBUG('c', "reading io char %c head %d taile %d cout %d\n", c, iobuf->head, iobuf->tail, iobuf->count);
        //DEBUG('c', "reading char %c\n", c);
        iobuf->head++;
        iobuf->head %= iobuf->size;
        iobuf->count--;   
             
        buf[read++] = c;
        ret = read;
        mt_sem_up(iobuf->nempty);
        if (c == EOF) {
            ret = -1;
            break;  
        } 
        if (iobuf->count == 0 && iobuf->lefttowrite == 0) {
            DEBUG('e', "breaking because lefttowrite 0 \n");
            break;
        }       
    }   
    DEBUG('c', "done reading\n");
    mt_sem_up(iobuf->read_serialize);
    return ret;
}

int io_buffer_write(IO_Buffer *iobuf, int size, char *buf) {
    int ret = 0;
    iobuf->lefttowrite = size;
    mt_sem_down(iobuf->write_serialize);
    DEBUG('c', "write call with size %d, string %s", size, buf);
    int write = 0;
    while (write < size) { 
        mt_sem_down(iobuf->nempty);
        /** Read the char from io buffer */
        iobuf->buf[iobuf->tail] = buf[write];
        DEBUG('c', "writing char to io buffer %c head %d tail %d count %d\n ", buf[write], iobuf->head, iobuf->tail, iobuf->count);
        write++;
        iobuf->lefttowrite--;
        iobuf->tail++;
        iobuf->tail %= iobuf->size;
        iobuf->count++;   
        ret = write; 
        DEBUG('l', "nreaders %d\n", iobuf->nreaders);
        if (iobuf->nreaders == 0) {
            exit(1);
        } 
        mt_sem_up(iobuf->nelem);
    }       
    DEBUG('c', "done writing\n");
    mt_sem_up(iobuf->write_serialize);
    return ret;
}


